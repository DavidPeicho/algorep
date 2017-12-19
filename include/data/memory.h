#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace algorep
{
  namespace
  {
    // Contains the clock of a message, as well as its size.
    using Pack = std::tuple<size_t, int>;
  }

  class Memory
  {
    public:
    std::string
    reserve(int node_rank, int nb_bytes);

    void
    release();

    void
    release(const std::string& id);

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

    inline std::unordered_map<std::string, std::tuple<Pack, Pack>>&
    history()
    {
      return this->history_;
    }

    private:
    std::unordered_map<std::string, std::vector<uint8_t>> data_;
    std::unordered_map<std::string, std::tuple<Pack, Pack>> history_;
  };
}  // namespace algorep
