#include <tuple>
#include <unordered_map>

namespace algorep
{
  class BaseElement { };

  template <typename T>
  class Element : public BaseElement
  {
    public:
      using Type = T;

    public:
      Element(size_t nb_values)
            : nb_values_{nb_values}
            , allocated_{false}
      { }

    public:
      inline void
      addId(const std::string& id, const std::tuple<size_t, size_t>& bounds)
      {
        this.var_[id] = bounds;
      }

    public:
      inline bool
      isAllocated()
      {
        return this.allocated_;
      }

    private:
      size_t nb_values_;
      bool allocated_;

      std::unordered_map<std::string, std::tuple<size_t, size_t>> vars_;
  };
}
