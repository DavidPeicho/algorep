#pragma once

#include <chrono>
#include <unordered_map>
#include <vector>

#include <constant/callback.h>
#include <data/element.h>

namespace algorep
{
  class Allocator
  {
    public:
    static inline Allocator*
    instance()
    {
      if (!instance_) instance_ = new Allocator();

      return instance_;
    }

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
    T*
    reduce(const Element<T>* elt);

    template <typename T>
    bool
    write(const Element<T>* elt, const T* data, size_t nb_elts = 0);

    void
    free(BaseElement* elt);

    void
    map(const BaseElement *elt, unsigned int data_type, unsigned int callback_id);

    public:

    inline void
    setMaxMemory(size_t nb_bytes)
    {
      this->max_memory_ = nb_bytes;
    }

    inline void
    setNbNodes(int nb_nodes)
    {
      this->nb_nodes_ = nb_nodes;
      for (int i = 0; i < nb_nodes_; ++i)
        this->memory_per_node_.push_back(this->max_memory_);
    }

    inline int
    getNbNodes() const
    {
      return this->nb_nodes_;
    }

    private:
    Allocator() : nb_nodes_(0), max_memory_(0) { }

    private:
    static Allocator *instance_;

    private:
    int nb_nodes_;
    size_t max_memory_;

    std::vector<unsigned long long> memory_per_node_;
  };
}  // namespace algorep

#include <data/allocator.hxx>
