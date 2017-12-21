#pragma once

#include <tuple>
#include <unordered_map>

namespace algorep
{
  namespace
  {
    // TODO: We could actually cache this value,
    // instead of recomputing it.
    int
    getRankFromId(const std::string& id)
    {
      return std::strtol(id.c_str(), nullptr, 10);
    }
  }

  class BaseElement
  {
    public:
    BaseElement(size_t nb_values, unsigned int atom_size)
        : nb_values_{nb_values}, atom_size_{atom_size}
    {
    }

    ~BaseElement(){};

    public:
    inline void
    addId(const std::string& id, const std::tuple<size_t, size_t>& bounds)
    {
      this->bounds_.push_back(bounds);
      this->ids_.push_back(id);
      this->int_ids_.push_back(getRankFromId(id));
    }

    public:
    inline const std::vector<std::tuple<size_t, size_t>>&
    getBounds() const
    {
      return this->bounds_;
    }

    inline const std::vector<std::string>&
    getIds() const
    {
      return this->ids_;
    }

    inline const std::vector<int>&
    getIntIds() const
    {
      return this->int_ids_;
    }

    inline size_t
    getNbValues() const
    {
      return this->nb_values_;
    }

    inline unsigned int
    getAtomSize() const
    {
      return this->atom_size_;
    }

    protected:
    size_t nb_values_;
    unsigned int atom_size_;

    std::vector<std::tuple<size_t, size_t>> bounds_;
    std::vector<std::string> ids_;
    std::vector<int> int_ids_;
  };

  template <typename T>
  class Element : public BaseElement
  {
    public:
    Element(size_t nb_values) : BaseElement(nb_values, sizeof(T)) {}
  };
}
