#include <data/allocator.h>

namespace algorep
{

  Allocator::Allocator(int nb_nodes, unsigned long long max_memory)
            : nb_nodes_{nb_nodes - 1}
  {
    for (int i = 0; i < nb_nodes_; ++i)
      this->memory_per_node_.push_back(max_memory);
  }

} // namespace algorep
