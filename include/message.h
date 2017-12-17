#pragma once

#include <vector>

#include <mpi/mpi.h>

namespace algorep
{
  namespace message
  {
    template <typename T>
    inline int
    send(const T *buffer, size_t nb_bytes,
        int dest, int tag, MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_bytes, MPI_BYTE, dest, tag,
                       MPI_COMM_WORLD, &request);
    }

    inline int
    send(const std::string& str, int dest,
         int tag, MPI_Request& request)
    {
      const char *buffer = str.c_str();
      size_t nb_bytes = str.length() + 1; // Do not forget the '\0'

      return send<char>(buffer, nb_bytes, dest, tag, request);
    }

    template<typename T>
    inline int
    rec_sync(int dest, int tag, int bytes, T *out)
    {
      return MPI_Recv(out, bytes, MPI_BYTE, dest,
                      tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    template<typename T>
    inline int
    rec_sync(int dest, int tag, MPI_Status &status, T **out)
    {
      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      *out = new T[nb_bytes];
      return message::rec_sync<T>(dest, tag, nb_bytes, *out);
    }

    template<typename T>
    inline int
    rec_sync(int dest, int tag, T **out)
    {
      MPI_Status status;
      MPI_Probe(dest, tag, MPI_COMM_WORLD, &status);

      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      *out = new T[nb_bytes];
      return message::rec_sync<T>(dest, tag, nb_bytes, *out);
    }
  } // namespace algorep
} // namespace algorep