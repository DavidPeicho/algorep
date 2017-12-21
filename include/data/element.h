#pragma once

#include <tuple>
#include <unordered_map>

/**
 * @file element.h
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace
  {
    // TODO: We could actually cache this value,
    // instead of recomputing it.
    /**
     * @brief 
     *
     * @param id
     *
     * @return 
     */
    int
    getRankFromId(const std::string& id)
    {
      return std::strtol(id.c_str(), nullptr, 10);
    }
  }

  /**
   * @brief 
   */
  class BaseElement
  {
    public:
    /**
     * @brief Constructor.
     *
     * @param nb_values
     * @param atom_size
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
     * @brief 
     *
     * @param id
     * @param bounds
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
     * @brief 
     *
     * @return 
     */
    inline const std::vector<std::tuple<size_t, size_t>>&
    getBounds() const
    {
      return this->bounds_;
    }

    /**
     * @brief 
     *
     * @return 
     */
    inline const std::vector<std::string>&
    getIds() const
    {
      return this->ids_;
    }

    /**
     * @brief 
     *
     * @return 
     */
    inline const std::vector<int>&
    getIntIds() const
    {
      return this->int_ids_;
    }

    /**
     * @brief 
     *
     * @return 
     */
    inline size_t
    getNbValues() const
    {
      return this->nb_values_;
    }

    /**
     * @brief 
     *
     * @return 
     */
    inline unsigned int
    getAtomSize() const
    {
      return this->atom_size_;
    }

    protected:
    /**
     * @brief 
     */
    size_t nb_values_;

    /**
     * @brief 
     */
    unsigned int atom_size_;

    /**
     * @brief 
     */
    std::vector<std::tuple<size_t, size_t>> bounds_;

    /**
     * @brief 
     */
    std::vector<std::string> ids_;

    /**
     * @brief 
     */
    std::vector<int> int_ids_;
  };

  /**
   * @brief 
   *
   * @tparam T
   */
  template <typename T>
  class Element : public BaseElement
  {
    public:
    /**
     * @brief 
     *
     * @param nb_values
     */
    Element(size_t nb_values) : BaseElement(nb_values, sizeof(T)) {}
  };
}
