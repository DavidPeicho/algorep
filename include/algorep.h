#pragma once

#include <functional>

#include <constant/callback.h>
#include <data/allocator.h>
#include <data/memory.h>

namespace algorep
{
  namespace
  {
    constexpr size_t MAX_MEMORY = 512 * 1024 * 1024;

    template <typename T>
    inline void
    applyCallback(uint8_t* input, size_t nb_elt, unsigned int call_id)
    {
      T* data = (T*)input;
      nb_elt = nb_elt / sizeof(T);

      for (size_t i = 0; i < nb_elt; ++i)
        algorep::callback::MAPS[call_id](&data[i]);
    }

    template <typename T>
    inline void
    applyReduce(const uint8_t* input, size_t nb_elt, unsigned int call_id,
                uint8_t* out)
    {
      const T* input_cast = (T*)input;
      T* out_cast = (T*)out;
      nb_elt = nb_elt / sizeof(T);
      for (size_t i = 0; i < nb_elt; ++i)
        algorep::callback::REDUCE[call_id](&input_cast[i], out_cast);
    }
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
