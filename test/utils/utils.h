// TODO: It would have been better to use a *real* test suite
// system, such as GoogleTest or libCheck.

#pragma once

#include <cstdlib>
#include <functional>
#include <iostream>
#include <vector>

#include <algorep.h>

using Allocator = algorep::Allocator;

namespace
{
  template <typename T>
  unsigned int
  finishTest(bool success, Allocator& allocator, algorep::Element<T>* var,
             T* read)
  {
    // Frees the network allocated array.
    delete[] read;
    // Frees local copy on the process.
    allocator.free(var);
    return !!success;
  }
}

template <typename T>
unsigned int
check_map(Allocator& allocator, std::vector<T>& in, unsigned int data_type,
          unsigned int callback_id, std::function<bool(T, T)> comp_func)
{
  size_t size = in.size();

  auto* my_var = allocator.reserve(in.size(), &in[0]);
  allocator.map(my_var, data_type, callback_id);

  auto* read = allocator.read(my_var);
  size_t i = 0;
  for (; i < size; ++i)
  {
    algorep::callback::CALLBACK_LIST[callback_id](&in[i]);
    if (!comp_func(in[i], read[i])) break;
  }

  return finishTest<T>(i == size, allocator, my_var, read);
}

template <typename T>
unsigned int
check(Allocator& allocator, const std::vector<T>& expected,
      std::function<bool(T, T)> comp_func)
{
  size_t size = expected.size();

  // Makes allocation on the Algorep Allocator.
  auto* my_variable = allocator.reserve<T>(size, &expected[0]);
  auto* read = allocator.read<T>(my_variable);

  size_t i = 0;
  for (; i < size; ++i)
  {
    if (!comp_func(expected[i], read[i])) break;
  }

  return finishTest<T>(i == size, allocator, my_variable, read);
}

template <typename T>
unsigned int
check(Allocator& allocator, std::function<bool(T, T)> comp_func)
{
  static constexpr size_t MAX_SIZE = 1000;
  size_t array_size = rand() % MAX_SIZE;

  // Fills the expected array with default values.
  std::vector<T> expected_arr(array_size);
  for (size_t i = 0; i < array_size; ++i) expected_arr[i] = rand();

  return check<T>(allocator, expected_arr, comp_func);
}

void
summary(unsigned int passed, unsigned int total, const char* title)
{
  std::cout << title << std::endl;
  std::cout << "Summary : " << passed << " / " << total << " passed!"
            << std::endl;
}
