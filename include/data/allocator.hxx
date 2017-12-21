#include <cstdlib>
#include <cstring>

#include <mpi/mpi.h>

#include <constant/constants.h>
#include <data/tag_data.h>
#include <message.h>

/**
 * @file allocator.hxx
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  template <typename T>
  Element<T>*
  Allocator::reserve(size_t nb_elements, const T* elt)
  {
    size_t nb_bytes = sizeof(T) * nb_elements;
    nb_bytes = nb_bytes;

    std::vector<std::tuple<unsigned int, size_t, size_t>> nodes;
    // First, we check if the size of the allocation can fit
    // on a single node.
    int i = 0;
    size_t start_idx = 0;
    size_t free_elt = nb_elements;
    for (; i < this->nb_nodes_; ++i)
    {
      auto max = (this->memory_per_node_[i] / sizeof(T));
      if (max < 1) continue;

      if (max >= free_elt)
      {
        nodes.push_back(
            std::make_tuple(i + 1, start_idx, start_idx + free_elt - 1));
        break;
      }
      nodes.push_back(std::make_tuple(i + 1, start_idx, start_idx + max - 1));
      free_elt -= max;
      start_idx += max;
    }

    if (nodes.size() == 0) return nullptr;

    auto* result = new Element<T>(nb_elements);
    // Sends allocation messages to each node containing
    // a part of the data (the data can be on only one node).
    for (const auto& node : nodes)
    {
      const auto node_id = std::get<0>(node);
      const auto& lower = std::get<1>(node);
      const auto& upper = std::get<2>(node);

      // Computes the number of bytes to send to the node.
      size_t bytes = sizeof(T) * (upper - lower + 1);

      MPI_Request req;
      message::send<T>(elt + lower, bytes, node_id, TAGS::ALLOCATION, req);
    }

    // Waits until every allocation is done.
    // Waiting here may give better throughput than just
    // waiting in the previous loop after a call to `send'.
    for (const auto& node : nodes)
    {
      const auto node_id = std::get<0>(node);
      const auto& lower = std::get<1>(node);
      const auto& upper = std::get<2>(node);

      char* id = nullptr;
      message::rec_sync(node_id, TAGS::ALLOCATION, &id);

      result->addId(std::string(id), std::make_tuple(lower, upper));

      size_t bytes = sizeof(T) * (upper - lower + 1);
      // TODO: normally, we should check that every allocation
      // has succeeded.
      this->memory_per_node_[node_id - 1] -= bytes;

      if (id != nullptr) delete[] id;
    }

    return result;
  }

  template <typename T>
  T*
  Allocator::read(const Element<T>* elt)
  {
    auto* result = new T[elt->getNbValues()];

    const auto& ids = elt->getIds();
    const auto& bounds = elt->getBounds();
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto& bound = bounds[i];
      const auto& id = ids[i];

      // int dest = getRankFromId(id);
      const int dest = elt->getIntIds()[i];

      // Asks the `dest' slave for a read.
      MPI_Request req;
      message::send(id, dest, TAGS::READ, req);

      // The result of the read is save in the `read' pointer.
      T* read = nullptr;
      message::rec_sync<T>(dest, TAGS::READ, &read);

      const auto& lower_bound = std::get<0>(bound);
      size_t count = (std::get<1>(bound) - lower_bound) + 1;

      // Copies the array from it's lower bound index.
      std::memcpy(result + lower_bound, read, count * sizeof(T));

      delete[] read;
    }

    return result;
  }

  template <typename T>
  bool
  Allocator::write(const Element<T>* elt, const T* data, size_t nb_elts)
  {
    // TODO: add atomic variable.
    static size_t CLOCK = 0;

    if (nb_elts > elt->getNbValues()) return false;
    nb_elts = (nb_elts == 0) ? elt->getNbValues() : nb_elts;

    const auto& ids = elt->getIds();
    const auto& bounds = elt->getBounds();

    // We will put the clock at the end of the message.
    // TODO: When data are to large, send a PRE_WRITE message
    // to avoid making a huge copy.
    std::vector<std::vector<uint8_t>> formatted(ids.size());
    for (unsigned int i = 0; i < ids.size() && nb_elts > 0; ++i)
    {
      const auto& id = ids[i];
      const auto& bound = bounds[i];
      const auto& lower = std::get<0>(bound);
      const auto& upper = std::get<1>(bound);

      size_t sub_nb_values = (upper - lower + 1);
      size_t data_bytes = sizeof(T) * sub_nb_values;
      if (nb_elts <= sub_nb_values)
      {
        data_bytes = sizeof(T) * nb_elts;
        nb_elts = 0;  // Makes the for loop stop after this iteration.
      }

      // Builds a new array to send, laying the data as follow:
      //    N bytes      22 bytes   sizeof (size_t)
      // [...Data...]   [...ID...]   [..Clock..]
      formatted[i].reserve(data_bytes + sizeof(size_t) + constant::ID_LEN);
      // Copies the `data' pointer to `formatted'
      std::memcpy(&formatted[i][0], data + lower, data_bytes);
      // Copies the id at the end. The ID will never be more than 22 char.
      std::memset(&formatted[i][0] + data_bytes, 0, constant::ID_LEN);
      std::memcpy(&formatted[i][0] + data_bytes, id.c_str(), id.length());
      // Copies the clock at the end of the data
      std::memcpy(&formatted[i][0] + data_bytes + constant::ID_LEN, &CLOCK,
                  sizeof(size_t));

      nb_elts -= sub_nb_values;
    }

    // Sends each part of the data to write
    // to each node involved.
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto& data = formatted[i];
      // The data are splitted linearly on clusters. If we find
      // a cluster that does not need data, we can safely assume
      // that the next ones are in the same state.
      if (data.capacity() == 0) break;

      // const int dest = getRankFromId(ids[i]);
      const int dest = elt->getIntIds()[i];

      // Asks the slave `dest' for a write.
      MPI_Request req;
      message::send<uint8_t>(&data[0], data.capacity(), dest, TAGS::WRITE, req);
    }

    // This part is super important. If we return directly,
    // the send may not be over, but the data of the `formatted'
    // vector will be flushed, which means that the data sent
    // to the nodes will be garbage.
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto& data = formatted[i];
      if (data.capacity() == 0) break;

      // const int dest = getRankFromId(ids[i]);
      const int dest = elt->getIntIds()[i];
      // TODO: It would have been better to use a completely
      // asynchronous system. However, it would need something
      // such as a ThreadPool that does busy waiting, and I don't
      // have that for now.
      uint8_t status = 0;
      message::rec_sync_ack(dest, TAGS::WRITE, status);

      if (!status) return false;
    }

    CLOCK++;
    return true;
  }

  template <typename T>
  Allocator*
  Allocator::map(const Element<T>* elt, unsigned int callback_id)
  {
    static constexpr int DATA_TYPE = callback::ElementType<T>::value;
    const auto& ids = elt->getIds();
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const int dest = elt->getIntIds()[i];

      std::string id = ids[i] + "-" + std::to_string(callback_id);
      id += "-" + std::to_string(DATA_TYPE);

      MPI_Request req;
      message::send(id, dest, TAGS::MAP, req);
    }

    for (size_t i = 0; i < ids.size(); ++i)
    {
      const int dest = elt->getIntIds()[i];

      uint8_t status = 0;
      message::rec_sync_ack(dest, TAGS::MAP, status);
    }

    return this;
  }

  template <typename T>
  T*
  Allocator::reduce(const Element<T>* elt, unsigned int callback_id, T init_val)
  {
    static constexpr unsigned int UINT_LEN = sizeof(unsigned int);
    static constexpr unsigned int DATA_LEN = 2 * UINT_LEN + 64;
    const auto& ids = elt->getIds();

    if (ids.size() == 0) return nullptr;

    // We send the message to the first cluster.
    // const int dest = getRankFromId(ids[0]);
    // const int last = getRankFromId(ids[ids.size() - 1]);
    const int dest = elt->getIntIds()[0];
    const int last = elt->getIntIds()[ids.size() - 1];

    // We will also send a string containing the chain
    // of every node to reach.
    // For now, we only select cluster when the previous one is full,
    // but with this technique, we can later update our policy to allocate
    // the memory without breaking the `reduce()' method.
    std::string nodes_list;
    for (size_t i = 0; i < ids.size(); ++i)
    {
      if (i == ids.size() - 1)
        nodes_list += ids[i];
      else
        nodes_list += ids[i] + "-";
    }

    // Sends the data with this layout:
    //  64 bytes       sizeof (uint)      sizeof (uint)        N
    // [ACCUMULATOR] [...DATA_TYPE...] [...CALLBACK_ID...]  [nodes]
    size_t nb_bytes = DATA_LEN + nodes_list.length() + 1;
    std::vector<uint8_t> data(nb_bytes);

    // Copies 64 bytes with initial accumulator value.
    std::memset(&data[0], 0, 64);
    std::memcpy(&data[0], &init_val, sizeof(T));
    // Copies data type
    std::memcpy(&data[0] + 64, &callback::ElementType<T>::value, UINT_LEN);
    std::memcpy(&data[0] + 64 + UINT_LEN, &callback_id, UINT_LEN);
    std::memcpy(&data[0] + 64 + 2 * UINT_LEN, nodes_list.c_str(),
                nodes_list.length() + 1);

    // Sends message to first node of the list.
    MPI_Request req;
    message::send<uint8_t>(&data[0], nb_bytes, dest, TAGS::REDUCE, req);

    // Waits for the message from the last node of the list.
    T* read = nullptr;
    message::rec_sync<T>(last, TAGS::REDUCE, &read);

    return read;
  }

}  // namespace algorep
