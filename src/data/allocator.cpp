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
  Allocator::free(BaseElement *elt)
  {
    const auto &vars = elt->getVariables();
    for (const auto &pair : vars)
    {
      int dest = getRankFromId(pair.first);
      const auto &lower = std::get<0>(pair.second);
      const auto &upper = std::get<1>(pair.second);

      // Asks the `dest' slave for a write.
      // This function will return as soon as the `pair.first'
      // string is copied, this will not wait until the receiver
      // acknowledge it, which is exactly what we want.
      message::send_sync(pair.first, dest, TAGS::FREE);

      // We basically consider that every message will arrive one day.
      // We can safely consider the memory as freed.
      size_t bytes = elt->getAtomSize() * (upper - lower + 1);
      this->memory_per_node_[dest - 1] += bytes;
    }
    delete elt;
  }
} // namespace algorep
