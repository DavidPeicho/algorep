#include <message.h>
#include <data/tag_data.h>

namespace algorep
{
  template <typename T>
  std::shared_ptr<Element<T>>
  Allocator::reserve(size_t nb_elements)
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
      message::send(&type_size, 1, i, TAGS::ALLOCATION, req);

      // Returns a newly created Element, which encapsulate an allocation
      // on the network. However, note that the allocation may have not been
      // performed yet.
      return std::make_shared<Element<T>>(nb_elements);
    }

    return nullptr;
  }

} // namespace algorep