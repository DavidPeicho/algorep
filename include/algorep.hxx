#include <cstdlib>

#include <mpi/mpi.h>

#include <algorep.h>
#include <data/constants.h>
#include <data/tag_data.h>

namespace algorep
{
  namespace
  {
    constexpr static uint8_t FAIL_STR = 0;

    void
    onAllocation(MPI_Status& status, Memory& memory, int rank)
    {
      MPI_Request req;
      int bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &bytes);

      // Allocation failed, we do not save it, and we return a fail.
      if (bytes == MPI_UNDEFINED)
      {
        message::send<uint8_t>(&FAIL_STR, 1, 0, TAGS::ALLOCATION, req);
        return;
      }

      // Allocates the data on the cluster.
      auto id = memory.reserve(rank, bytes);
      message::rec_sync<uint8_t>(0, TAGS::ALLOCATION, bytes,
                                 &memory.get(id)[0]);

      // This is used in the `onWrite' callback.
      // This will allow to handle old write messages
      // that come after newer messages.
      memory.history()[id] = std::make_tuple(0, 0);

      // Sends an acknowledge to the master.
      message::send(id, 0, TAGS::ALLOCATION, req);
    }

    void
    onRead(MPI_Status& status, const Memory& memory)
    {
      // Gets back ID sent by the master.
      char* id = nullptr;
      message::rec_sync(0, TAGS::READ, status, &id);

      // Sends the data to the master.
      MPI_Request req;
      const auto& data = memory.getConst(std::string(id));
      message::send(&data[0], data.size(), 0, TAGS::READ, req);

      delete[] id;
    }

    void
    onWrite(MPI_Status& status, Memory& memory)
    {
      // Retrieves the data from the master.
      // The data lays out like this:
      //    N bytes      22 bytes   sizeof (size_t)
      // [...Data...]   [...ID...]   [..Clock..]
      uint8_t *data = nullptr;
      int bytes = 0;
      message::rec_sync<uint8_t>(0, TAGS::WRITE, status, &bytes, &data);

      // TODO: Handle error, which should not happen.
      // In the case we have 0 bytes, it means that some data
      // has been lost on the network.
      if (bytes == 0) return;

      int data_size = bytes - constant::ID_LEN - sizeof (size_t);
      int clock_offset = bytes - sizeof (size_t);

      size_t clock = *((size_t*)(data + clock_offset));
      const char* id_cstr = (char *)(data + data_size);
      std::string id(id_cstr);

      std::cout << "bytes = " << bytes << std::endl;
      std::cout << "received clock: " << clock << std::endl;
      std::cout << "received id: " << id << std::endl;

      auto &var = memory.get(id);

      auto &tuple = memory.history()[id];
      auto &old_clock = std::get<0>(tuple);
      auto &old_size = std::get<1>(tuple);

      // We can safely write the data directly, without being
      // afraid of erasing message arrived in a wrong order.
      if (old_size == 0)
      {
        std::memcpy(&var[0], data, data_size);
        old_clock = clock;
        old_size = data_size;
        delete[] data;
        return;
      }

      // The arriving message was sent after the previous
      // one saved in the history.
      if (clock > old_clock)
      {
        std::memcpy(&var[0], data, data_size);
        // A completed flush has been done,
        // we can reset the history.
        /*if (data_size == var.capacity())
        {
          old_size = 0;
          old_clock = 0;
        }*/
      }
      else
      {
        if (data_size > old_size)
        {
          int bytes = (data_size - old_size);
          std::memcpy(&var[0] + bytes, data, bytes);
        }

      }

      delete[] data;
    }

    void
    onQuit(Memory& memory)
    {
      memory.release();
      MPI_Finalize();
      std::exit(0);
    }

    void
    onFree(MPI_Status& status, Memory& memory)
    {
      // Gets back ID sent by the master.
      char* id = nullptr;
      message::rec_sync(0, TAGS::FREE, status, &id);

      // Frees the memory associated to the `id' ID.
      memory.release(std::string(id));

      delete[] id;
    }
  }

  template <typename T>
  T
  run(const std::function<T(Allocator&)> callback, size_t max_memory)
  {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int nb_nodes = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_nodes);

    // Instanciate an allocator, which will only be used in the master,
    // But every node theoritically could allocate some space to another
    // node.
    // As we are using a Master-Slave system, the Allocator simply uses
    // the ID 0 by default for the master, but it can easily be changed.
    Allocator allocator(nb_nodes, max_memory);

    if (rank == 0) return callback(allocator);

    Memory memory;
    MPI_Status status;

    while (true)
    {
      // Gets back informaton about *any* message.
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      switch (status.MPI_TAG)
      {
        case TAGS::ALLOCATION:
          onAllocation(status, memory, rank);
          break;
        case TAGS::READ:
          onRead(status, memory);
          break;
        case TAGS::WRITE:
          onWrite(status, memory);
          break;
        case TAGS::FREE:
          onFree(status, memory);
          break;
        case TAGS::QUIT:
          onQuit(memory);
          break;
      }
      // TODO:
      // * Handle write
    }

    return 1;
  }
}  // namespace algorep
