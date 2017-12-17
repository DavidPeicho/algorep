#include <tuple>
#include <unordered_map>

namespace algorep
{
  class BaseElement
  {
    public:
      using Type = int;
  };

  template <typename T>
  class Element : public BaseElement
  {
    public:
      using Type = T;

    public:
      Element(size_t nb_values)
            : nb_values_{nb_values}
      { }

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

      inline bool
      isAllocated()
      {
        return this.allocated_;
      }

    private:
      size_t nb_values_;

      std::unordered_map<std::string, std::tuple<size_t, size_t>> vars_;
  };
}
