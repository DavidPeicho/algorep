#include <algorep.h>
#include <message.h>

namespace algorep
{
  namespace
  {
    void
    setPack(size_t clock, int size, std::tuple<size_t, int>& out)
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
      memory.history()[id] =
          std::make_tuple(std::make_tuple(0, 0), std::make_tuple(0, 0));

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
      uint8_t* data = nullptr;
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

      int data_size = bytes - constant::ID_LEN - sizeof(size_t);
      int clock_offset = bytes - sizeof(size_t);

      size_t clock = *((size_t*)(data + clock_offset));
      const char* id_cstr = (char*)(data + data_size);
      std::string id(id_cstr);

      auto& var = memory.get(id);
      // Contains the oldest largest message received.
      auto& old_pack = std::get<0>(memory.history()[id]);
      auto& new_pack = std::get<1>(memory.history()[id]);

      // Message is the newest, we can safely erase
      // previously written data.
      if (clock > std::get<0>(new_pack))
      {
        std::memcpy(&var[0], data, data_size);
        setPack(clock, data_size, new_pack);
        // A completed flush has been done,
        // we can reset the history.
        if ((size_t)data_size == var.capacity()) setPack(0, 0, old_pack);
      }
      else if (data_size > std::get<1>(old_pack) ||
               clock > std::get<0>(old_pack))
      {
        const auto& start = std::get<1>(new_pack);
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

    void
    onMap(MPI_Status& status, Memory& memory)
    {
      char* data_cstr = nullptr;
      message::rec_sync(0, TAGS::MAP, status, &data_cstr);

      std::string data(data_cstr);

      MPI_Request req;
      if (data.size() == 0)
      {
        // Sends an acknowledge to the master.
        message::send<uint8_t>(&constant::FAIL, 1, 0, TAGS::MAP, req);
        return;
      }

      size_t sep = data.find('-');
      size_t sep2 = data.find('-', sep + 1);
      std::string id = data.substr(0, sep);

      unsigned int callback_id = strtol(data_cstr + sep + 1, NULL, 10);
      unsigned int data_type = strtol(data_cstr + sep2 + 1, NULL, 10);

      size_t nb_elt = memory.get(id).capacity();
      auto* var_data = &memory.get(id)[0];
      switch (data_type)
      {
        case DataType::USHORT:
          applyCallback<unsigned short>(var_data, nb_elt, callback_id);
          break;
        case DataType::SHORT:
          applyCallback<short>(var_data, nb_elt, callback_id);
          break;
        case DataType::UINT:
          applyCallback<unsigned int>(var_data, nb_elt, callback_id);
          break;
        case DataType::INT:
          applyCallback<int>(var_data, nb_elt, callback_id);
          break;
        case DataType::ULONG:
          applyCallback<unsigned long>(var_data, nb_elt, callback_id);
          break;
        case DataType::LONG:
          applyCallback<long>(var_data, nb_elt, callback_id);
          break;
        case DataType::FLOAT:
          applyCallback<float>(var_data, nb_elt, callback_id);
          break;
        case DataType::DOUBLE:
          applyCallback<double>(var_data, nb_elt, callback_id);
          break;
      }

      // Sends an acknowledge to the master.
      message::send_sync<uint8_t>(&constant::SUCCESS, 1, 0, TAGS::MAP);
      delete[] data_cstr;
    }

  }

  void
  onReduce(MPI_Status& status, Memory& memory)
  {
    static constexpr unsigned int UINT_LEN = sizeof (unsigned int);
    // Sends the data with this layout:
    //  64 bytes       sizeof (uint)      sizeof (uint)        N
    // [ACCUMULATOR] [...DATA_TYPE...] [...CALLBACK_ID...]  [nodes]
    uint8_t * data = nullptr;
    int bytes = 0;
    message::rec_sync<uint8_t>(
      status.MPI_SOURCE, TAGS::REDUCE, status, &bytes, &data
    );

    std::string nodes_list((char*)(data + 64 + UINT_LEN * 2));
    std::string curr_id = nodes_list;
    size_t sep = nodes_list.find('-');
    if (sep != std::string::npos) curr_id = curr_id.substr(0, sep);

    auto &vec = memory.get(curr_id);
    auto* var_data = &vec[0];
    size_t nb_elt = vec.capacity();
    unsigned int data_type = *((unsigned int*)(data + 64));
    unsigned int call_id = *((unsigned int*)(data + 64 + UINT_LEN));

    size_t nb_bytes_type = DataTypeToSize[data_type];
    switch (data_type)
    {
      case DataType::USHORT:
        applyReduce<unsigned short>(var_data, nb_elt, call_id, data);
        break;
      case DataType::SHORT:
        applyReduce<short>(var_data, nb_elt, call_id, data);
        break;
      case DataType::UINT:
        applyReduce<unsigned int>(var_data, nb_elt, call_id, data);
        break;
      case DataType::INT:
        applyReduce<int>(var_data, nb_elt, call_id, data);
        break;
      case DataType::ULONG:
        applyReduce<unsigned long>(var_data, nb_elt, call_id, data);
        break;
      case DataType::LONG:
        applyReduce<long>(var_data, nb_elt, call_id, data);
        break;
      case DataType::FLOAT:
        applyReduce<float>(var_data, nb_elt, call_id, data);
        break;
      case DataType::DOUBLE:
        applyReduce<double>(var_data, nb_elt, call_id, data);
        break;
    }

    // The id is only composed of a '\0'. We are on the last node
    // of the chain, we can send the result to the master.
    if (sep == std::string::npos)
    {
      // Sends an acknowledge to the master.
      message::send_sync<uint8_t>(data, nb_bytes_type, 0, TAGS::REDUCE);
      delete[] data;
      return;
    }

    // We are now going to remove the next node
    // from the nodes list, and send it the message.
    const char *cstr = nodes_list.c_str() + sep + 1;
    int next_dest = strtol(cstr, NULL, 10);

    size_t nb_bytes = nodes_list.length() - sep;
    std::memcpy(data + 64 + UINT_LEN * 2, cstr, nb_bytes);

    // Sends the message to the next node of the chain.
    message::send_sync<uint8_t>(
      data, 64 + UINT_LEN * 2 + nb_bytes, next_dest, TAGS::REDUCE
    );

    delete[] data;
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

    if (nb_nodes < 2)
    {
      std::string error = "number of nodes in cluster should be >= 2!";
      throw std::invalid_argument("algorep: " + error);
    }

    // As we are using a Master-Slave system, the Allocator simply uses
    // the ID 0 by default for the master, but it can easily be changed.
    // Allocator allocator(nb_nodes, max_memory);
    Allocator::instance()->setMaxMemory(max_memory);
    Allocator::instance()->setNbNodes(nb_nodes - 1);

    if (rank == 0) return callback();

    Memory memory;
    MPI_Status status;

    while (true)
    {
      // Gets back informaton about *any* message.
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
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
        case TAGS::MAP:
          onMap(status, memory);
          break;
        case TAGS::REDUCE:
          onReduce(status, memory);
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
    auto* allocator = Allocator::instance();
    // Sends `quit' message to children
    MPI_Request req;
    for (int i = 0; i < allocator->getNbNodes(); ++i)
      MPI_Isend(NULL, 0, MPI_BYTE, i + 1, TAGS::QUIT, MPI_COMM_WORLD, &req);

    MPI_Finalize();
  }

  void
  terminate()
  {
    auto* allocator = Allocator::instance();
    delete allocator;
  }
}  // namespace algorep
