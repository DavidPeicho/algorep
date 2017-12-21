/**
 * @file simple_map_reduce.cpp
 * @brief This sample shows how to simply use the allocator to make map and reduce
 * operations.
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include <algorep.h>

using namespace algorep::callback;

/**
 * @brief 
 *
 * @param memStatus
 */
void
printMemoryStatus(const std::vector<unsigned long long> &memStatus)
{
  std::cout << "- Memory status: \n";
  for (size_t i = 0; i < memStatus.size(); ++i)
    std::cout << "    - Child " << i << " -> "
              << memStatus[i] << " bytes free\n";
  std::cout << std::endl;
}

/**
 * @brief 
 *
 * @tparam T
 * @param allocator
 * @param in
 *
 * @return 
 */
template <typename T>
algorep::Element<T>*
reserve(algorep::Allocator *allocator, const std::vector<T>& in)
{
  size_t nb_bytes = sizeof (T) * in.size();
  auto *var = allocator->reserve<T>(in.size(), &in[0]);
  std::cout << "Reserving " << nb_bytes << " bytes on slaves ";
  for (size_t i = 0; i < var->getIntIds().size(); ++i)
    std::cout << "`" << var->getIntIds()[i] << "' ";
  std::cout << std::endl;

  return var;
}

/**
 * @brief 
 */
void
run()
{
  auto* allocator = algorep::Allocator::instance();
  const auto &memory_status = allocator->getMemoryStatus();

  std::cout << "\n[INFO]\n"
            << "- Number of slaves: "
            << memory_status.size() << "\n";
  printMemoryStatus(memory_status);

  static constexpr unsigned int SIZE = 30000;

  // Generates an array of size `SIZE' and fill
  // it with large random doubles.
  std::vector<double> test(SIZE);
  std::cout << "Generated random array:\n";
  for (unsigned int i = 0; i < 20; ++i)
  {
    std::srand(i + time(nullptr));
    double normalized = ((double)(std::rand()) / (double)(RAND_MAX - 1));
    test[i] = (normalized * (double)SIZE) - ((double)SIZE * 0.5);
    std::cout << test[i] << " ";
    if ((i + 1) % 10 == 0) std::cout << std::endl;
  }
  std::cout << "...\n" << std::endl;
  for (unsigned int i = 20; i < SIZE; ++i)
  {
    std::srand(i + time(nullptr));
    double normalized = ((double)(std::rand()) / (double)(RAND_MAX - 1));
    test[i] = (normalized * (double)SIZE) - ((double)SIZE * 0.5);
  }

  // Allocates the array on the slaves.
  auto *var = reserve<double>(allocator, test);
  std::cout << "\n" << std::endl;
  printMemoryStatus(memory_status);

  // Sums all the element before applying the map.
  // It will be used to compare the result after the map.
  double *sum = allocator->reduce<double>(var, ReduceID::D_SUM, 0.0);

  std::cout << "Mapping on positive values..." << std::endl;

  // Maps each of the element pointed by the `var'
  // variable, to its absolute value counterpart.
  allocator->map<double>(var, MapID::D_ABS);

  // Reads it back to see that it actually worked.
  double *tmp = allocator->read<double>(var);
  for (unsigned int i = 0; i < 20; ++i)
  {
    std::cout << tmp[i] << " ";
    if ((i + 1) % 10 == 0) std::cout << std::endl;
  }
  std::cout << "...\n" << std::endl;
  delete[] tmp;

  double *sum2 = allocator->reduce<double>(var, ReduceID::D_SUM, 0.0);
  std::cout << "Sums all the values together..." << std::endl;
  std::cout << "-> sum before map = " << *sum << std::endl;
  std::cout << "-> sum after map = " << *sum2 << std::endl;
  delete[] sum;
  delete[] sum2;

  allocator->free(var);
  // Super important call, forgeting this will make
  // the slaves wait indefinitely.
  algorep::finalize();
}

int
main(int argc, char** argv)
{
  algorep::init(argc, argv);

  const auto& callback = std::function<void()>(run);
  algorep::run(callback, 200000);

  // This is in charge of liberating some allocated
  // memory.
  algorep::terminate();
}
