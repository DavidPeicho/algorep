#pragma once

#include <functional>

#include <data/allocator.h>
#include <data/memory.h>

namespace algorep
{
  void
  init(int argc, char **argv);

  template <typename T>
  T
  run(const std::function<T(Allocator&)> callback);

  void
  release();

}

#include <algorep.hxx>
