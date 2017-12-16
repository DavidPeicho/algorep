#include <algorep.h>
#include <iostream>
#include <mpi/mpi.h>

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

    while (true)
    {
      break;
    }

    std::cout << "child" << std::endl;

    return 1;
  }
}  // namespace algorep
