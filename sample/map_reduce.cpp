#include <algorep.h>
#include <iostream>

using namespace algorep::callback;

void
run()
{
  auto* allocator = algorep::Allocator::instance();

  double toto [5] = {
    0.0001, 1.0, 1.1, 1.4, 0.76
  };
  auto *var = allocator->reserve<double>(5, toto);

  double *val = allocator->reduce<double>(var, ReduceID::D_SUM);
  std::cout << *val << std::endl;

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback, 22);

  // This is in charge of liberating some allocated
  // memory.
  algorep::terminate();
}
