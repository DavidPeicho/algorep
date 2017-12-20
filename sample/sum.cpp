#include <algorep.h>
#include <iostream>

using Allocator = algorep::Allocator;

void
test()
{
  auto *allocator = Allocator::instance();

  int toto[5] = {5, 4, 3, 2, 1};
  auto my_variable = allocator->reserve<int>(5, toto);

  auto* read = allocator->read<int>(my_variable);
  for (unsigned i = 0; i < 5; ++i)
    std::cout << "Result = " << *(read + i) << std::endl;

  // Write test
  int toto2[5] = {-1, 2, -3, 8, 2};
  allocator->write(my_variable, toto2);

  auto* read2 = allocator->read<int>(my_variable);
  for (int i = 0; i < 5; ++i)
    std::cout << "Result 2 = " << read2[i] << std::endl;

  allocator->map(my_variable, algorep::DataType::INT, 12);

  delete[] read;
  delete[] read2;

  read2 = allocator->read<int>(my_variable);
  for (int i = 0; i < 5; ++i)
    std::cout << "Result 2 = " << read2[i] << std::endl;

  // Frees the network allocated array.
  allocator->free(my_variable);
  // Frees local copy on the process.

  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(test);
  algorep::run(callback);

  // This is in charge of liberating some allocated
  // memory.
  algorep::terminate();
}
