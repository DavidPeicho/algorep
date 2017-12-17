#include <cstdlib>

#include <mpi/mpi.h>

#include <algorep.h>
#include <data/tag_data.h>

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
    MPI_Status status;

    while (true)
    {
      // Gets back informaton about *any* message.
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      switch (status.MPI_TAG)
      {
        case TAGS::ALLOCATION:
        {
          int bytes = 0;
          MPI_Get_count(&status, MPI_BYTE, &bytes);
          if ( bytes != MPI_UNDEFINED )
          {
            // Allocates the data on the cluster.
            auto id = memory.reserve(rank, bytes);
            message::rec_sync<uint8_t>(0, TAGS::ALLOCATION, bytes, &memory.get(id)[0]);
            // Sends an acknowledge to the master.
            MPI_Request req;
            message::send(id, 0, TAGS::ALLOCATION, req);
          }
          break;
        }
        case TAGS::READ:
        {
          char *id = nullptr;
          message::rec_sync(0, TAGS::READ, status, &id);

          const auto &data = memory.get(std::string(id));

          std::cout << "Reading on child: " << ((int*)&data[0])[0] << std::endl;

          // Sends data to the master.
          MPI_Request req;
          message::send(&data[0], data.size(), 0, TAGS::READ, req);

          delete[] id;
          break;
        }
        case TAGS::QUIT:
          MPI_Finalize();
          std::exit(0);
          break;
      }
      // TODO:
      // * Handle allocation (do not forget to set the variable ID).
      // * Handle read
      // * Handle write
    }

    return 1;
  }
}  // namespace algorep
