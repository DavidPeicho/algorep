#include <data/memory.h>

namespace algorep
{
  std::string
  Memory::reserve(int node_rank, int nb_bytes)
  {
    static unsigned long long ID = 0;
    std::string id = std::to_string(node_rank) + "_" + std::to_string(ID);

    std::vector<uint8_t> vec(nb_bytes);
    this->data_[id] = vec;

    return id;
  }
} // namespace algorep
