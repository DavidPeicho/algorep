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

    for (int i = 1; i < allocator.getNbNodes(); ++i)
      message::send(nullptr, i, TAGS::QUIT, req);

    MPI_Finalize();
  }
}  // namespace algorep
