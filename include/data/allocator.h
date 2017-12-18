#pragma once

#include <unordered_map>
#include <vector>

#include <data/element.h>

namespace algorep
{
  class Allocator
  {
    public:
      Allocator(int nb_nodes, unsigned long long max_memory);

    public:
      template <typename T>
      Element<T>*
      reserve(size_t nb_elements, const T *elt);

      template<typename T>
      T*
      read(const Element<T> *elt);

      void
      update();

      void
      free();

    public:
      inline int
      getNbNodes() const
      {
        return this->nb_nodes_;
      }

    private:
      int nb_nodes_;
      std::vector<unsigned long long> memory_per_node_;
  };
} // namespace algorep

#include <data/allocator.hxx>
