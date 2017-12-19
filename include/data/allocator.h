#pragma once

#include <chrono>
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
    // TODO: add equivalent of malloc / calloc.
    // For now, you are obliged to use a typed allocation,
    // it would be nice to only allocate space in bytes.
    template <typename T>
    Element<T>*
    reserve(size_t nb_elements, const T* elt);

    template <typename T>
    T*
    read(const Element<T>* elt);

    template <typename T>
    bool
    write(const Element<T>* elt, const T* data, size_t nb_elts = 0);

    void
    update();

    void
    free(BaseElement* elt);

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
}  // namespace algorep

#include <data/allocator.hxx>
