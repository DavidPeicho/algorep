#include <cstdlib>
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
    size_t type_size = sizeof (T);
    unsigned long long nb_bytes = type_size * nb_elements;

    // First, we check if the size of the allocation can fit
    // on a single node.
    int i = 0;
    for (; i < this->nb_nodes_; ++i )
    {
      const auto mem = this->memory_per_node_[i];
      if (nb_bytes <= mem)
        break;
    }

    // The data can fit in a single node, we make the allocation.
    if (i != this->nb_nodes_)
    {
      // The master is node 0, we skip it.
      i += 1;

      MPI_Request req;
      message::send<T>(elt, nb_bytes, i, TAGS::ALLOCATION, req);

      // Wait for allocation to be done on node.
      MPI_Status status;
      MPI_Probe(i, TAGS::ALLOCATION, MPI_COMM_WORLD, &status);

      int str_len = 0;
      char *id = new char[str_len];

      MPI_Get_count(&status, MPI_BYTE, &str_len);
      message::rec_sync<char>(i, TAGS::ALLOCATION, str_len, id);

      auto element = new Element<T>(nb_elements);
      element->addId(std::string(id), std::make_tuple(0, 0));

      delete[] id;

      // Returns a newly created Element, which encapsulate an allocation
      // on the network. However, note that the allocation may have not been
      // performed yet.
      return element;
    }

    return nullptr;
  }

  template <typename T>
  T*
  Allocator::read(const Element<T> *elt)
  {
    //auto *result = new T[elt->getNbValues()];
    // The allocations is not over yet, we will have to wait
    // for it to complete.
    const auto &vars = elt->getVariables();
    for (const auto &pair : vars)
    {
      int dest = getRankFromId(pair.first);
      MPI_Request req;
      message::send(pair.first, dest, TAGS::READ, req);
      T *result = nullptr;
      message::rec_sync<T>(dest, TAGS::READ, &result);
      return result;
    }

    // Hopefully, RVO is here to speed up the process.
    return nullptr;
  }

} // namespace algorep