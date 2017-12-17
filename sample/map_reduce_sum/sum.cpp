#include <iostream>
#include <algorep.h>

using Allocator = algorep::Allocator;

int test(Allocator& allocator)
{
  int toto[5] = {5, 4, 3, 2, 1};
  auto my_variable = allocator.reserve<int>(5, toto);

  std::cout << "Allocation done!" << std::endl;

  auto *read = allocator.read<int>(my_variable);

  for (unsigned i = 0; i < 5; ++i)
    std::cout << "Result = " << *(read + i) << std::endl;

  delete[] read;

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::release(allocator);
  return 1;
}

int main(int argc, char **argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<int(Allocator&)>(test);
  run(callback);

}
