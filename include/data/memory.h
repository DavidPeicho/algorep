#pragma once

#include <unordered_map>

namespace algorep
{
  class Memory
  {
    private:
      std::unordered_map<std::string, void*> data_;
  };
} // namespace algorep
