#include <algorep.h>

namespace algorep
{
  void
  init(int argc, char **argv)
  {
    MPI_Init(&argc, &argv);
  }

  void
  release()
  {
    MPI_Finalize();
  }
}  // namespace algorep
