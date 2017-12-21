#include <algorep.h>
#include <iostream>

#include "utils/utils.h"

using namespace algorep::callback;

void
run()
{
  auto* allocator = algorep::Allocator::instance();
  unsigned int tests_passed = 0;

  auto int_comp = [](auto a, auto b) { return a == b; };
  auto float_comp = [](auto a, auto b) {
    constexpr float EPSILON = 0.000001;
    return a >= b - EPSILON && a <= b + EPSILON;
  };

  std::vector<double> toto({
    0.0001, 1.0, 1.1, 1.4, 0.76
  });
  tests_passed += check_reduce<double>(
    *allocator, toto, ReduceID::D_SUM, 0.0, float_comp
  );
  tests_passed += check_reduce<double>(
    *allocator, toto, ReduceID::D_SUM, 9182.5, float_comp
  );
  std::vector<int> toto2({
    0, 0, 0, 1, 4, 9, -1293, 1000, 292, 1, 100, -99, 13, -10, 15
  });
  tests_passed += check_reduce<int>(
    *allocator, toto2, ReduceID::I_SUM, 0, int_comp
  );
  std::vector<unsigned short> toto3({
    100, 4024, 10000, 500, 111
  });
  tests_passed += check_reduce<unsigned short>(
    *allocator, toto3, ReduceID::US_SUM, 1, int_comp
  );
  std::vector<unsigned long> toto4({
    1000000, 14842044, 193133, 0, 0, 0, 0, 0, 0, 101933, 1119203, 11222, 11
  });
  tests_passed += check_reduce<unsigned long>(
    *allocator, toto4, ReduceID::UL_SUM, 13033, int_comp
  );

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();

  summary(tests_passed, 5, "> Reduces hardcoded <");
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback, 64);

  // This is in charge of liberating some allocated
  // memory.
  algorep::terminate();
}
