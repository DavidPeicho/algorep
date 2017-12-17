#include <mpi/mpi.h>

#include <algorep.h>

namespace algorep
{
  template <typename T>
  T
  run(const std::function<T(Allocator&)> callback)
  {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int nb_nodes = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_nodes);

    Allocator allocator(nb_nodes);

    if (rank == 0)
      return callback(allocator);

    Memory memory;

    while (true)
    {
      // TODO:
      // * Handle allocation (do not forget to set the variable ID).
      // * Handle read
      // * Handle write
      break;
    }

    std::cout << "child" << std::endl;

    return 1;
  }
}  // namespace algorep
