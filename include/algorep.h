#pragma once

#include <functional>

#include <data/allocator.h>
#include <data/memory.h>

namespace algorep
{
  namespace
  {
    constexpr size_t MAX_MEMORY = 512 * 1024 * 1024;
  }

  void
  init(int argc, char** argv);

  void
  run(const std::function<void()> callback, size_t max_memory = MAX_MEMORY);

  void
  registerOperation();

  void
  finalize();

  void
  terminate();
}
