#include <algorep.h>
#include <iostream>

#include "utils/utils.h"

using Allocator = algorep::Allocator;
using namespace algorep::callback;

void
run()
{
  auto* allocator = Allocator::instance();
  unsigned int tests_passed = 0;

  auto int_comp = [](auto a, auto b) { return a == b; };
  auto float_comp = [](auto a, auto b) {
    constexpr float EPSILON = 0.000001;
    return a >= b - EPSILON && a <= b + EPSILON;
  };

  std::vector<int> toto({5, 4, 3, 2, 1, -1, 0, -100});
  tests_passed += check_map<int>(*allocator, toto, MapID::I_POW, int_comp);

  std::vector<float> toto2({5.3, 0.01, 0.006, 1039393.9, 0.0});
  tests_passed += check_map<float>(*allocator, toto2, MapID::F_POW, float_comp);

  std::vector<double> toto3({5.30000, 0.000000001, 4981914.1111, 0.09});
  tests_passed += check_map<double>(*allocator, toto3, MapID::D_POW, float_comp);

  std::vector<unsigned long> toto4(
      {13949404, 1003, 1, 0, 10000000, 100, 9999999});
  tests_passed += check_map<unsigned long>(*allocator, toto4, MapID::UL_POW, int_comp);

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();

  summary(tests_passed, 4, "> Print few mappings <");
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback);

  // This is in charge of liberating some allocated
  // memory.
  algorep::terminate();
}
