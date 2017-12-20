#include <algorep.h>
#include <message.h>

namespace algorep
{
  namespace
  {
    void
    setPack(size_t clock, int size, std::tuple<size_t, int> &out)
    {
      std::get<0>(out) = clock;
      std::get<1>(out) = size;
    }

    void
    onAllocation(MPI_Status& status, Memory& memory, int rank)
    {
      MPI_Request req;
      int bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &bytes);

      // Allocation failed, we do not save it, and we return a fail.
      if (bytes == MPI_UNDEFINED)
      {
        message::send<uint8_t>(&constant::FAIL, 1, 0, TAGS::ALLOCATION, req);
        return;
      }

      // Allocates the data on the cluster.
      auto id = memory.reserve(rank, bytes);
      message::rec_sync<uint8_t>(0, TAGS::ALLOCATION, bytes,
                                 &memory.get(id)[0]);

      // This is used in the `onWrite' callback.
      // This will allow to handle old write messages
      // that come after newer messages.
      memory.history()[id] = std::make_tuple(
        std::make_tuple(0, 0), std::make_tuple(0, 0)
      );

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
      // In the case we have 0 bytes, it means
      // that some data has been lost on the network.
      MPI_Request req;
      if (bytes == 0)
      {
        message::send<uint8_t>(&constant::FAIL, 1, 0, TAGS::WRITE, req);
        return;
      }

      int data_size = bytes - constant::ID_LEN - sizeof (size_t);
      int clock_offset = bytes - sizeof (size_t);

      size_t clock = *((size_t*)(data + clock_offset));
      const char* id_cstr = (char *)(data + data_size);
      std::string id(id_cstr);

      auto &var = memory.get(id);
      // Contains the oldest largest message received.
      auto &old_pack = std::get<0>(memory.history()[id]);
      auto &new_pack = std::get<1>(memory.history()[id]);

      // Message is the newest, we can safely erase
      // previously written data.
      if (clock > std::get<0>(new_pack))
      {
        std::memcpy(&var[0], data, data_size);
        setPack(clock, data_size, new_pack);
        // A completed flush has been done,
        // we can reset the history.
        if ((size_t)data_size == var.capacity())
          setPack(0, 0, old_pack);
      }
      else if (data_size > std::get<1>(old_pack)
               || clock > std::get<0>(old_pack))
      {
        const auto &start = std::get<1>(new_pack);
        std::memcpy(&var[0] + start, data + start, data_size - start);
        if (clock < std::get<0>(old_pack) || std::get<0>(old_pack) == 0)
          setPack(clock, data_size, old_pack);
      }

      // Sends an acknowledge to the master.
      message::send<uint8_t>(&constant::SUCCESS, 1, 0, TAGS::WRITE, req);

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

  void
  init(int argc, char** argv)
  {
    MPI_Init(&argc, &argv);
  }

  void
  run(const std::function<void()> callback, size_t max_memory)
  {
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int nb_nodes = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_nodes);

    // As we are using a Master-Slave system, the Allocator simply uses
    // the ID 0 by default for the master, but it can easily be changed.
    //Allocator allocator(nb_nodes, max_memory);
    Allocator::instance()->setMaxMemory(max_memory);
    Allocator::instance()->setNbNodes(nb_nodes - 1);

    if (rank == 0) return callback();

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
    }
  }

  void
  finalize()
  {
    auto *allocator = Allocator::instance();
    // Sends `quit' message to children
    MPI_Request req;
    for (int i = 0; i < allocator->getNbNodes(); ++i)
      MPI_Isend(NULL, 0, MPI_BYTE, i + 1, TAGS::QUIT, MPI_COMM_WORLD, &req);

    MPI_Finalize();
  }

  void
  terminate()
  {
    auto *allocator = Allocator::instance();
    delete allocator;
  }
}  // namespace algorep
