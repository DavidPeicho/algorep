#pragma once

#include <functional>

#include <constant/callback.h>
#include <data/allocator.h>
#include <data/memory.h>

/**
 * @file algorep.h
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace
  {
    /**
     * @brief Default value for max_memory parameter of run function.
     */
    constexpr size_t MAX_MEMORY = 512 * 1024 * 1024;

    /**
     * @brief Apply mapping callback on each element.
     *
     * @tparam T Type of element.
     * @param input Data used as T*.
     * @param nb_elt Number of elements in input.
     * @param call_id Callback to use.
     */
    template <typename T>
    inline void
    applyCallback(uint8_t* input, size_t nb_elt, unsigned int call_id)
    {
      T* data = (T*)input;
      nb_elt = nb_elt / sizeof(T);

      for (size_t i = 0; i < nb_elt; ++i)
        algorep::callback::MAPS[call_id](&data[i]);
    }

    /**
     * @brief Apply reduce callbacks on each element.
     *
     * @tparam T Type of element.
     * @param input Data used as const T*.
     * @param nb_elt Number of elements in input.
     * @param call_id Callback to use.
     * @param out Accumulator.
     */
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

  /**
   * @brief Initialize environment.
   *
   * @param argc Number of arguments.
   * @param argv Arguments.
   */
  void
  init(int argc, char** argv);

  /**
   * @brief Execution of a node (react to messages).
   *
   * @param callback Master behavior.
   * @param max_memory Maximum memory per node.
   */
  void
  run(const std::function<void()> callback, size_t max_memory = MAX_MEMORY);

  /**
   * @brief Terminate execution environment.
   */
  void
  finalize();

  /**
   * @brief Free the unique allocator instance.
   */
  void
  terminate();
}
