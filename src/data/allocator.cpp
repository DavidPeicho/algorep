#include <data/allocator.h>

namespace algorep
{
  Allocator::Allocator(int nb_nodes, unsigned long long max_memory)
      : nb_nodes_{nb_nodes - 1}
  {
    for (int i = 0; i < nb_nodes_; ++i)
      this->memory_per_node_.push_back(max_memory);
  }

  void
  Allocator::free(BaseElement* elt)
  {
    const auto& ids = elt->getIds();
    const auto& bounds = elt->getBounds();

    for (size_t i = 0; i < ids.size(); ++i)
    {
      const auto& id = ids[i];
      const auto& bound = bounds[i];

      int dest = getRankFromId(id);
      const auto& lower = std::get<0>(bound);
      const auto& upper = std::get<1>(bound);

      // Asks the `dest' slave for a write.
      // This function will return as soon as the `pair.first'
      // string is copied, this will not wait until the receiver
      // acknowledge it, which is exactly what we want.
      message::send_sync(id, dest, TAGS::FREE);

      // We basically consider that every message will arrive one day.
      // We can safely consider the memory as freed.
      size_t bytes = elt->getAtomSize() * (upper - lower + 1);
      this->memory_per_node_[dest - 1] += bytes;
    }
    delete elt;
  }
}  // namespace algorep
