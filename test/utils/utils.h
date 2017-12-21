/**
 * @file utils.h
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

// TODO: It would have been better to use a *real* test suite
// system, such as GoogleTest or libCheck.

#pragma once

#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <vector>

#include <algorep.h>

using Allocator = algorep::Allocator;

namespace
{
  /**
   * @brief 
   *
   * @tparam T
   * @param success
   * @param allocator
   * @param var
   * @param read
   *
   * @return 
   */
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

/**
 * @brief 
 *
 * @tparam T
 * @param allocator
 * @param in
 * @param callback_id
 * @param init_val
 * @param comp_func
 *
 * @return 
 */
template <typename T>
unsigned int
check_reduce(Allocator& allocator, std::vector<T>& in, unsigned int callback_id,
             T init_val, std::function<bool(T, T)> comp_func)
{
  size_t size = in.size();

  auto* my_var = allocator.reserve<T>(in.size(), &in[0]);
  T* result = allocator.reduce<T>(my_var, callback_id, init_val);

  T val = init_val;
  for (size_t i = 0; i < size; ++i)
    algorep::callback::REDUCE[callback_id](&in[i], &val);

  return finishTest(comp_func(val, *result), allocator, my_var, result);
}

/**
 * @brief 
 *
 * @tparam T
 * @param allocator
 * @param in
 * @param callback_id
 * @param comp_func
 *
 * @return 
 */
template <typename T>
unsigned int
check_map(Allocator& allocator, std::vector<T>& in,
          unsigned int callback_id, std::function<bool(T, T)> comp_func)
{
  size_t size = in.size();

  auto* my_var = allocator.reserve(in.size(), &in[0]);
  allocator.map<T>(my_var, callback_id);

  auto* read = allocator.read(my_var);
  size_t i = 0;
  for (; i < size; ++i)
  {
    algorep::callback::MAPS[callback_id](&in[i]);
    if (!comp_func(in[i], read[i])) break;
  }

  return finishTest<T>(i == size, allocator, my_var, read);
}

/**
 * @brief 
 *
 * @tparam T
 * @param allocator
 * @param expected
 * @param comp_func
 *
 * @return 
 */
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

/**
 * @brief 
 *
 * @tparam T
 * @param allocator
 * @param comp_func
 *
 * @return 
 */
template <typename T>
unsigned int
check(Allocator& allocator, std::function<bool(T, T)> comp_func)
{
  static constexpr size_t MAX_SIZE = 1000;
  size_t array_size = rand() % MAX_SIZE;

  // Fills the expected array with default values.
  std::vector<T> expected_arr(array_size);
  for (size_t i = 0; i < array_size; ++i)
  {
    std::srand(i + time(nullptr));
    expected_arr[i] = rand();
  }

  return check<T>(allocator, expected_arr, comp_func);
}

/**
 * @brief 
 *
 * @param passed
 * @param total
 * @param title
 */
void
summary(unsigned int passed, unsigned int total, const char* title)
{
  std::cout << title << std::endl;
  std::cout << "Summary : " << passed << " / " << total << " passed!"
            << std::endl;
}
