#pragma once

#include <tuple>
#include <unordered_map>

/**
 * @file element.h
 * @brief File containing the main object of the API: Element.
 * Giving to the user an `Element` allows to hide the data kept by the master
 * regarding the slave. It also improves the cleanness of the API.
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace
  {
    /**
     * @brief Get the rank of a node from its identifier.
     *
     * @param id String identifier.
     *
     * @return Rank of the node.
     */
    int
    getRankFromId(const std::string& id)
    {
      return std::strtol(id.c_str(), nullptr, 10);
    }
  }

  /**
   * @brief This class allows us to make type-erasure when freing a variable
   * of type Element<T>.
   */
  class BaseElement
  {
    public:
    /**
     * @brief Constructor.
     *
     * @param nb_values Number of elements in data.
     * @param atom_size Size of one element.
     */
    BaseElement(size_t nb_values, unsigned int atom_size)
        : nb_values_{nb_values}, atom_size_{atom_size}
    {
    }

    /**
     * @brief Destructor.
     */
    ~BaseElement(){};

    public:
    /**
     * @brief Track a new chunk of data on a specific node.
     *
     * @param id Node identifier.
     * @param bounds Bounds of the chunk of data on the node.
     */
    inline void
    addId(const std::string& id, const std::tuple<size_t, size_t>& bounds)
    {
      this->bounds_.push_back(bounds);
      this->ids_.push_back(id);
      this->int_ids_.push_back(getRankFromId(id));
    }

    public:
    /**
     * @brief Get the bounds of the split data on every node.
     *
     * @return Bounds of chunks for each node.
     */
    inline const std::vector<std::tuple<size_t, size_t>>&
    getBounds() const
    {
      return this->bounds_;
    }

    /**
     * @brief Get nodes identifiers where the data is.
     *
     * @return Nodes identifiers.
     */
    inline const std::vector<std::string>&
    getIds() const
    {
      return this->ids_;
    }

    /**
     * @brief Get nodes identifiers where the data is as integers.
     *
     * @return Nodes identifiers as integers.
     */
    inline const std::vector<int>&
    getIntIds() const
    {
      return this->int_ids_;
    }

    /**
     * @brief Get number of elements in data.
     *
     * @return Number of elements.
     */
    inline size_t
    getNbValues() const
    {
      return this->nb_values_;
    }

    /**
     * @brief Get size of one element. This is only used when freeing memory.
     *
     * @return Size of one element.
     */
    inline unsigned int
    getAtomSize() const
    {
      return this->atom_size_;
    }

    protected:
    /**
     * @brief Number of element.
     */
    size_t nb_values_;

    /**
     * @brief Size of one element.
     */
    unsigned int atom_size_;

    /**
     * @brief Bounds of chunks of data on each node.
     */
    std::vector<std::tuple<size_t, size_t>> bounds_;

    /**
     * @brief Nodes identifiers.
     */
    std::vector<std::string> ids_;

    /**
     * @brief Nodes identifiers as integers.
     */
    std::vector<int> int_ids_;
  };

  /**
   * @brief Contains data regarding chunks on slaves. This is the main object
   * used in the API, and allows the Allocator to keep track of how variables
   * are dispatched on the network.
   *
   * @tparam T
   */
  template <typename T>
  class Element : public BaseElement
  {
    public:
    /**
     * @brief Constructor.
     *
     * @param nb_values Number of elements in data.
     */
    Element(size_t nb_values) : BaseElement(nb_values, sizeof(T)) {}
  };
}
