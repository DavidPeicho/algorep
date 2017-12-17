#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <data/element.h>

namespace algorep
{
  namespace
  {
    constexpr unsigned long long MAX_MEMORY = 512 * 1024 * 1024;
  }

  class Allocator
  {
    public:
      Allocator(int nb_nodes, unsigned long long max_memory = MAX_MEMORY);

    public:
      template <typename T>
      std::shared_ptr<Element<T>>
      reserve(size_t nb_elements);

      void
      read();

      void
      update();

      void
      free();

    private:
      int nb_nodes_;
      std::vector<unsigned long long> memory_per_node_;
      std::unordered_map<std::string, std::shared_ptr<BaseElement>> variables_;
  };
} // namespace algorep

#include <data/allocator.hxx>
