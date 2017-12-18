#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace algorep
{
  class Memory
  {
    public:
      std::string
      reserve(int node_rank, int nb_bytes);

      void
      release();

    public:
      inline std::vector<uint8_t>&
      get(const std::string& id)
      {
        return this->data_[id];
      }

      inline const std::vector<uint8_t>&
      getConst(const std::string& id) const
      {
        return this->data_.at(id);
      }

    private:
      std::unordered_map<std::string, std::vector<uint8_t>> data_;
  };
} // namespace algorep
