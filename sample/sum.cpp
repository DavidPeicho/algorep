#include <algorep.h>
#include <iostream>

using Allocator = algorep::Allocator;

int
test(Allocator& allocator)
{
  int toto[5] = {5, 4, 3, 2, 1};
  auto my_variable = allocator.reserve<int>(5, toto);

  auto* read = allocator.read<int>(my_variable);
  for (unsigned i = 0; i < 5; ++i)
    std::cout << "Result = " << *(read + i) << std::endl;

  // Write test
  int toto2[5] = {1, 2, 3, 8, 2};
  allocator.write(my_variable, toto2);

  // Frees the network allocated array.
  allocator.free(my_variable);
  // Frees local copy on the process.
  delete[] read;

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::release(allocator);

  return 1;
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<int(Allocator&)>(test);
  run(callback);
}
