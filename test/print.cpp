#include "utils/utils.h"

int
run()
{
  auto* allocator = algorep::Allocator::instance();

  auto int_comp = [](auto a, auto b) { return a == b; };
  auto float_comp = [](auto a, auto b) {
    constexpr float EPSILON = 0.000001;
    return a >= b - EPSILON && a <= b + EPSILON;
  };

  unsigned int tests_passed = 0;

  std::vector<int> a({1, 2, 3, 4, 5});
  tests_passed += check<int>(*allocator, a, int_comp);

  std::vector<int> b({1, 2, 3, 4, 5, 6, 7, 8, 100});
  tests_passed += check<int>(*allocator, b, int_comp);

  std::vector<int> c({1, 2, 3, 4, 5, 6, 7, 8, 100, -10334, -99, -928440});
  tests_passed += check<int>(*allocator, c, int_comp);

  std::vector<float> d({1.4f, 2.1f, 19.9f, 500.5f, 0.001f, 0.094f});
  tests_passed += check<float>(*allocator, d, float_comp);

  std::vector<double> e({0.00003, 0.00001, 1983039303.0, 1.100089, 0.90139403});
  tests_passed += check<double>(*allocator, e, float_comp);

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();

  summary(tests_passed, 4, "> Print hardcoded <");

  return 1;
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback);

  algorep::terminate();

  return 0;
}
