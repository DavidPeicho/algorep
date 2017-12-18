#include <cstdlib>
#include <cstring>
#include <mpi/mpi.h>

#include <message.h>
#include <data/tag_data.h>

namespace algorep
{
  namespace
  {
    int
    getRankFromId(const std::string &id)
    {
      return std::strtol(id.c_str(), nullptr, 10);
    }
  }

  template <typename T>
  Element<T>*
  Allocator::reserve(size_t nb_elements, const T *elt)
  {
    size_t nb_bytes = sizeof (T) * nb_elements;
    nb_bytes = nb_bytes;

    std::vector<std::tuple<unsigned int, size_t, size_t>> nodes;
    // First, we check if the size of the allocation can fit
    // on a single node.
    int i = 0;
    size_t start_idx = 0;
    size_t free_elt = nb_elements;
    for (; i < this->nb_nodes_; ++i )
    {
      elt = elt;
      auto max = (this->memory_per_node_[i] / sizeof (T));
      if (max < 1)
        continue;

      if (max >= free_elt)
      {
        nodes.push_back(std::make_tuple(i + 1, start_idx, start_idx + free_elt - 1));
        break;
      }

      nodes.push_back(std::make_tuple(i + 1, start_idx, start_idx + max - 1));
      free_elt -= max;
      start_idx += max;
    }

    if (nodes.size() == 0)
      return nullptr;

    auto *result = new Element<T>(nb_elements);
    // Sends allocation messages to each node containing
    // a part of the data (the data can be on only one node).
    for (const auto& node : nodes)
    {
      const auto node_id = std::get<0>(node);
      const auto &lower = std::get<1>(node);
      const auto &upper = std::get<2>(node);

      // Computes the number of bytes to send to the node.
      size_t bytes = sizeof (T) * (upper - lower + 1);

      MPI_Request req;
      message::send<T>(elt + lower, bytes, node_id, TAGS::ALLOCATION, req);
    }

    // Waits until every allocation is done.
    // Waiting here may give better throughput than just
    // waiting in the previous loop after a call to `send'.
    for (const auto& node : nodes)
    {
      const auto node_id = std::get<0>(node);
      const auto &lower = std::get<1>(node);
      const auto &upper = std::get<2>(node);

      char *id = nullptr;
      message::rec_sync(node_id, TAGS::ALLOCATION, &id);

      result->addId(std::string(id), std::make_tuple(lower, upper));

      size_t bytes = sizeof (T) * (upper - lower + 1);
      // TODO: normally, we should check that every allocation
      // has succeeded.
      this->memory_per_node_[node_id - 1] -= bytes;
    }

    return result;
  }

  template <typename T>
  T*
  Allocator::read(const Element<T> *elt)
  {
    auto *result = new T[elt->getNbValues()];

    // The allocations is not over yet, we will have to wait
    // for it to complete.
    const auto &vars = elt->getVariables();
    for (const auto &pair : vars)
    {
      int dest = getRankFromId(pair.first);

      // Asks the `dest' slave for a read.
      MPI_Request req;
      message::send(pair.first, dest, TAGS::READ, req);

      // The result of the read is save in the `read' pointer.
      T *read = nullptr;
      message::rec_sync<T>(dest, TAGS::READ, &read);

      const auto &lower_bound = std::get<0>(pair.second);
      size_t count = (std::get<1>(pair.second) - lower_bound) + 1;

      std::memcpy(result + lower_bound, read, count * sizeof (T));

      delete[] read;
    }

    // Hopefully, RVO is here to speed up the process.
    return result;
  }

} // namespace algorep