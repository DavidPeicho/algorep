#include <algorep.h>
#include <message.h>

namespace algorep
{
  void
  init(int argc, char **argv)
  {
    MPI_Init(&argc, &argv);
  }

  void
  release(Allocator& allocator)
  {
    MPI_Request req;
    for (int i = 0; i < allocator.getNbNodes() + 1; ++i)
      MPI_Isend(NULL, 0, MPI_BYTE, i, TAGS::QUIT,
                MPI_COMM_WORLD, &req);

    MPI_Finalize();
  }
}  // namespace algorep
