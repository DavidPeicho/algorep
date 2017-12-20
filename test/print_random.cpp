#include "utils/utils.h"

void
run()
{
  auto* allocator = algorep::Allocator::instance();

  constexpr unsigned int NB_TESTS = 100;

  auto int_comp = [](auto a, auto b) { return a == b; };
  auto float_comp = [](auto a, auto b) {
    constexpr float EPSILON = 0.0001;
    return a >= b - EPSILON && a <= b + EPSILON;
  };

  unsigned int nb_total_tests = NB_TESTS * 4;
  unsigned int tests_passed = 0;

  // Launches `NB_TEST' tests on int values.
  for (unsigned i = 0; i < NB_TESTS; ++i)
    tests_passed += check<int>(*allocator, int_comp);
  // Launches `NB_TEST' tests on float values.
  for (unsigned i = 0; i < NB_TESTS; ++i)
    tests_passed += check<float>(*allocator, float_comp);
  // Launches `NB_TEST' tests on double values.
  for (unsigned i = 0; i < NB_TESTS; ++i)
    tests_passed += check<double>(*allocator, float_comp);
  // Launches `NB_TEST' tests on size_t values.
  for (unsigned i = 0; i < NB_TESTS; ++i)
    tests_passed += check<size_t>(*allocator, int_comp);

  algorep::finalize();

  summary(tests_passed, nb_total_tests, "> Print random <");
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback);
}
