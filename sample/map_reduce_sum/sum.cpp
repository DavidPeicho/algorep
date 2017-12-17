#include <iostream>
#include <algorep.h>

using Allocator = algorep::Allocator;

int test(Allocator& allocator)
{
  int toto = 5;
  auto my_variable = allocator.reserve<int>(1, &toto);

  std::cout << "Allocation done!" << std::endl;

  auto *read = allocator.read<int>(my_variable);

  std::cout << "Result = " << *read << std::endl;

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
