#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

#include <algorep.h>

using Allocator = algorep::Allocator;

static size_t g_TEST_NB = 0;

template <typename T>
unsigned int
check(Allocator &allocator,
      const std::vector<T>& expected, std::function<bool(T, T)> comp_func)
{
  size_t size = expected.size();

  std::cout << "Test " << g_TEST_NB++ << " with size `"
            << size << "'" << std::endl;

  // Makes allocation on the Algorep Allocator.
  auto *my_variable = allocator.reserve<T>(size, &expected[0]);
  const auto *read = allocator.read<T>(my_variable);

  size_t i = 0;
  for (; i < size; ++i)
  {
    if (!comp_func(expected[i], read[i]))
      break;
  }

  delete[] read;
  delete my_variable;

  unsigned int status = !!(i == size);
  std::cout << "[Status]: "
            << (status ? "PASSED" : "FAILED")
            << std::endl;

  return status;
}

template <typename T>
unsigned int
check(Allocator &allocator, std::function<bool(T, T)> comp_func)
{
  static constexpr size_t MAX_SIZE = 1000;
  size_t array_size = rand() % MAX_SIZE;

  // Fills the expected array with default values.
  std::vector<T> expected_arr(array_size);
  for (size_t i = 0; i < array_size; ++i)
    expected_arr[i] = rand();

  return check<T>(allocator, expected_arr, comp_func);
}
