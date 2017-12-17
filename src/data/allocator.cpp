#include <data/allocator.h>

namespace algorep
{

  Allocator::Allocator(int nb_nodes, unsigned long long max_memory)
            : nb_nodes_{nb_nodes}
  {
    this->memory_per_node_.reserve(nb_nodes_ - 1);
    std::fill(
      this->memory_per_node_.begin(),
      this->memory_per_node_.end(),
      max_memory
    );
  }

} // namespace algorep
