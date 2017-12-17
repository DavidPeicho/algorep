#pragma once

#include <mpi/mpi.h>

namespace algorep
{
  namespace message
  {
    //
    // Sending integer value(s).
    //
    inline int
    send(const int *buffer, size_t nb_elements,
        int dest, int tag, MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_elements, MPI_INT, dest, tag,
                       MPI_COMM_WORLD, &request);
    }
    //
    // Sending float value(s).
    //
    inline int
    send(const float *buffer, size_t nb_elements,
        int dest, int tag, MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_elements, MPI_FLOAT, dest, tag,
                       MPI_COMM_WORLD, &request);
    }
    //
    // Sending double value(s).
    //
    inline int
    send(const double *buffer, size_t nb_elements,
        int dest, int tag, MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_elements, MPI_DOUBLE, dest, tag,
                       MPI_COMM_WORLD, &request);
    }

    //
    // Sending size_t value(s).
    //
    inline int
    send(const size_t *buffer, size_t nb_elements,
        int dest, int tag, MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_elements, MPI_UNSIGNED_LONG_LONG, dest, tag,
                       MPI_COMM_WORLD, &request);
    }
  } // namespace algorep
} // namespace algorep