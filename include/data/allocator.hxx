#include <cstdlib>
#include <cstring>

#include <mpi/mpi.h>

#include <data/constants.h>
#include <data/tag_data.h>
#include <message.h>

namespace algorep
{
  namespace
  {
    int
    getRankFromId(const std::string& id)
    {
      return std::strtol(id.c_str(), nullptr, 10);
    }
  }

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
      elt = elt;
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

      int dest = getRankFromId(id);

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
    std::vector<MPI_Request> requests(ids.size());
    std::vector<std::vector<uint8_t>> formatted(ids.size());
    for (unsigned int i = 0; i < ids.size() && nb_elts > 0; ++i)
    {
      const auto &id = ids[i];
      const auto& bound = bounds[i];
      const auto &lower = std::get<0>(bound);
      const auto &upper = std::get<1>(bound);

      size_t sub_nb_values = (upper - lower + 1);
      size_t data_bytes = sizeof (T) * sub_nb_values;
      if (nb_elts <= sub_nb_values)
      {
        data_bytes = sizeof (T) * nb_elts;
        nb_elts = 0; // Makes the for loop stop after this iteration.
      }

      formatted[i].reserve(data_bytes + sizeof (size_t) + constant::ID_LEN);
      // Copies the `data' pointer to `formatted'
      std::memcpy(&formatted[i][0], data, data_bytes);
      // Copies the id at the end. The ID will never be more than 22 char.
      std::memset(&formatted[i][0] + data_bytes, 0, constant::ID_LEN);
      std::memcpy(&formatted[i][0] + data_bytes, id.c_str(), id.length());
      // Copies the clock at the end of the data
      std::memcpy(&formatted[i][0] + data_bytes + constant::ID_LEN, &CLOCK, sizeof (size_t));

      nb_elts -= sub_nb_values;
    }

    // Sends each part of the data to write
    // to each node involved.
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto &data = formatted[i];
      // The data are splitted linearly on clusters. If we find
      // a cluster that does not need data, we can safely assume
      // that the next ones are in the same state.
      if (data.capacity() == 0) break;

      const int dest = getRankFromId(ids[i]);
      // Asks the slave `dest' for a write.
      message::send<uint8_t>(&data[0], data.capacity(), dest, TAGS::WRITE, requests[i]);
    }

    // This part is super important. If we return directly,
    // the send may not be over, but the data of the `formatted'
    // vector will be flushed, which means that the data sent
    // to the nodes will be garbage.
    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto &data = formatted[i];
      if (data.capacity() == 0) break;

      // MPI_Wait(&requests[i], MPI_STATUS_IGNORE);
      const int dest = getRankFromId(ids[i]);
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

}  // namespace algorep
