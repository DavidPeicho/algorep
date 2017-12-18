#pragma once

#include <functional>

#include <data/allocator.h>
#include <data/memory.h>

namespace algorep
{
  namespace
  {
    constexpr unsigned long long MAX_MEMORY = 512 * 1024 * 1024;
  }

  void
  init(int argc, char **argv);

  template <typename T>
  T
  run(const std::function<T(Allocator&)> callback, size_t max_mem = MAX_MEMORY);

  void
  release(Allocator& allocator);

}

#include <algorep.hxx>
