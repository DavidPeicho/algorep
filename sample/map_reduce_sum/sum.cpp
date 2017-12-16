#include <iostream>
#include <algorep.h>

using Allocator = algorep::Allocator;

int test(Allocator& allocator)
{
  allocator.reserve();
  std::cout << "Master" << std::endl;
  return 1;
}

int main(int argc, char **argv)
{

  algorep::init(argc, argv);

  const auto& callback = std::function<int(Allocator&)>(test);
  run(callback);

  algorep::release();
}
