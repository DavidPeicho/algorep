#pragma once

namespace algorep
{
  class Allocator
  {
    public:
      Allocator(int nb_nodes);

    public:
      void
      reserve();

      void
      read();

      void
      update();

      void
      free();

    private:
      int nb_nodes_;
  };
} // namespace algorep
