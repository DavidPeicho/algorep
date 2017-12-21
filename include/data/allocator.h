#pragma once

#include <chrono>
#include <unordered_map>
#include <vector>

#include <constant/callback.h>
#include <data/element.h>

/**
 * @file allocator.h
 * @brief Base file describing the Allocator API.
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  /**
   * @brief Singleton.
   */
  class Allocator
  {
    public:
    /**
     * @brief Get a unique instance. Allocate it at first call.
     *
     * @return Pointer to unique instance.
     */
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
    /**
     * @brief Reserve shared memory.
     *
     * @tparam T Type of element.
     * @param nb_elements Number of elements to reserve space for.
     * @param elt Default value(s).
     *
     * @return Wrapping Element on location etc.
     */
    template <typename T>
    Element<T>*
    reserve(size_t nb_elements, const T* elt);

    /**
     * @brief Read shared memory.
     *
     * @tparam T Type of element.
     * @param elt Where to read.
     *
     * @return Pointer on queried data.
     */
    template <typename T>
    T*
    read(const Element<T>* elt);

    /**
     * @brief Write into shared memory.
     *
     * @tparam T Type of element.
     * @param elt Where to write.
     * @param data Value(s) to write.
     * @param nb_elts Number of elements to write from data.
     *
     * @return Whether the operation was successul.
     */
    template <typename T>
    bool
    write(const Element<T>* elt, const T* data, size_t nb_elts = 0);

    /**
     * @brief Free the passed argument and the underlying shared memory.
     *
     * @param elt Where to free.
     */
    void
    free(BaseElement* elt);

    /**
     * @brief Apply mapping callback on shared memory.
     *
     * @tparam T Type of element.
     * @param elt What to map.
     * @param callback_id Callback to use.
     *
     * @return Pointer to this instance.
     */
    template <typename T>
    Allocator*
    map(const Element<T>* elt, unsigned int callback_id);

    /**
     * @brief Apply reducing callback on shared memory.
     *
     * @tparam T Type of element.
     * @param elt What to reduce.
     * @param callback_id Callback to use.
     * @param init_val Default value for the accumulator.
     *
     * @return Pointer to result value.
     */
    template <typename T>
    T*
    reduce(const Element<T>* elt, unsigned int callback_id, T init_val = 0);

    public:
    /**
     * @brief Get current memory status per node.
     *
     * @return Current available memory per node.
     */
    inline const std::vector<unsigned long long>&
    getMemoryStatus() const
    {
      return this->memory_per_node_;
    }

    /**
     * @brief Set the maximum memory per node.
     *
     * @param nb_bytes Maximum number of bytes per node.
     */
    inline void
    setMaxMemory(size_t nb_bytes)
    {
      this->max_memory_ = nb_bytes;
    }

    /**
     * @brief Set the number of nodes in the network with an initial amount
     * of memory.
     *
     * @param nb_nodes Number of nodes in the network.
     */
    inline void
    setNbNodes(int nb_nodes)
    {
      this->nb_nodes_ = nb_nodes;
      for (int i = 0; i < nb_nodes_; ++i)
        this->memory_per_node_.push_back(this->max_memory_);
    }

    /**
     * @brief Get the number of nodes in the network.
     *
     * @return Number of nodes in the network.
     */
    inline int
    getNbNodes() const
    {
      return this->nb_nodes_;
    }

    private:
    /**
     * @brief Constructor.
     */
    Allocator() : nb_nodes_(0), max_memory_(0) {}

    private:
    /**
     * @brief Unique instance.
     */
    static Allocator* instance_;

    private:
    /**
     * @brief Number of nodes in the network.
     */
    int nb_nodes_;

    /**
     * @brief Maximum usable memory per node.
     */
    size_t max_memory_;

    /**
     * @brief Current available memory per node.
     */
    std::vector<unsigned long long> memory_per_node_;
  };
}  // namespace algorep

#include <data/allocator.hxx>
