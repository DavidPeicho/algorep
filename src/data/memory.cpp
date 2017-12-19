#include <data/memory.h>

namespace algorep
{
  std::string
  Memory::reserve(int node_rank, int nb_bytes)
  {
    static unsigned long long ID = 0;
    std::string id = std::to_string(node_rank) + "_" + std::to_string(ID++);

    std::vector<uint8_t> vec(nb_bytes);
    this->data_[id] = vec;

    return id;
  }

  void
  Memory::release()
  {
    for (auto& pair : this->data_) pair.second.clear();
  }

  void
  Memory::release(const std::string& id)
  {
    if (this->data_.count(id))
    {
      this->data_[id].clear();
      this->data_.erase(id);
    }
  }
}  // namespace algorep
