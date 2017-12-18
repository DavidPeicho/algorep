#pragma once

#include <tuple>
#include <unordered_map>

namespace algorep
{
  class BaseElement
  {
    public:
      BaseElement(size_t nb_values, unsigned int atom_size)
            : nb_values_{nb_values}
            , atom_size_{atom_size}
      { }

      ~BaseElement() { };

    public:
      inline void
      addId(const std::string& id, const std::tuple<size_t, size_t>& bounds)
      {
        this->vars_[id] = bounds;
      }

    public:
      inline const std::unordered_map<std::string, std::tuple<size_t, size_t>>&
      getVariables() const
      {
        return this->vars_;
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
      std::unordered_map<std::string, std::tuple<size_t, size_t>> vars_;
  };

  template <typename T>
  class Element : public BaseElement
  {
    public:
      Element(size_t nb_values)
          : BaseElement(nb_values, sizeof (T))
      { }
  };
}
