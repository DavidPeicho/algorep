#pragma once

#include <vector>

#include <mpi/mpi.h>

/**
 * @file message.h
 * @brief Contains wrapper above the OpenMPI API. This is used to simplify
 * the overuse pattern (wait for number of bytes, apply sync receive).
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace message
  {
    /**
     * @brief Send a non-blocking message to a particular node.
     *
     * @tparam T Type of element.
     * @param buffer Data to send.
     * @param nb_bytes Size of buffer.
     * @param dest Destination node.
     * @param tag Operation identifier.
     * @param request MPI handle.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    send(const T* buffer, size_t nb_bytes, int dest, int tag,
         MPI_Request& request)
    {
      return MPI_Isend(buffer, nb_bytes, MPI_BYTE, dest, tag, MPI_COMM_WORLD,
                       &request);
    }

    /**
     * @brief Send a non-blocking message to a particular node.
     *
     * @param str Data to send.
     * @param dest Destination node.
     * @param tag Operation identifier.
     * @param request MPI handle.
     *
     * @return MPI error code.
     */
    inline int
    send(const std::string& str, int dest, int tag, MPI_Request& request)
    {
      const char* buffer = str.c_str();
      size_t nb_bytes = str.length() + 1;  // Do not forget the '\0'

      return send<char>(buffer, nb_bytes, dest, tag, request);
    }

    /**
     * @brief Send a blocking message to a particular node.
     *
     * @tparam T Type of element.
     * @param buffer Data to send.
     * @param nb_bytes Size of buffer.
     * @param dest Destination node.
     * @param tag Operation identifier.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    send_sync(const T* buffer, size_t nb_bytes, int dest, int tag)
    {
      return MPI_Send(buffer, nb_bytes, MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    }

    /**
     * @brief Send a blocking message to a particular node.
     *
     * @param str Data to send.
     * @param dest Destination node.
     * @param tag Operation identifier.
     *
     * @return MPI error code.
     */
    inline int
    send_sync(const std::string& str, int dest, int tag)
    {
      const char* buffer = str.c_str();
      size_t nb_bytes = str.length() + 1;  // Do not forget the '\0'
      return send_sync<char>(buffer, nb_bytes, dest, tag);
    }

    /**
     * @brief Blocking receive of a message from a particular node.
     *
     * @param src Source node.
     * @param tag Operation identifier.
     * @param out Data to retrieve.
     *
     * @return MPI error code.
     */
    inline int
    rec_sync_ack(int src, int tag, uint8_t& out)
    {
      return MPI_Recv(&out, 1, MPI_BYTE, src, tag, MPI_COMM_WORLD,
                      MPI_STATUS_IGNORE);
    }

    /**
     * @brief Blocking receive of a message from a particular node.
     *
     * @tparam T Type of element.
     * @param src Source node.
     * @param tag Operation identifier.
     * @param bytes Maximum number of bytes to receive.
     * @param out Data to retrieve.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    rec_sync(int src, int tag, int bytes, T* out)
    {
      return MPI_Recv(out, bytes, MPI_BYTE, src, tag, MPI_COMM_WORLD,
                      MPI_STATUS_IGNORE);
    }

    /**
     * @brief Blocking receive of a message from a particular node.
     *
     * @tparam T Type of element.
     * @param src Source node.
     * @param tag Operation identifier.
     * @param status Status object to retrieve data size.
     * @param out Data to retrieve.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    rec_sync(int src, int tag, MPI_Status& status, T** out)
    {
      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      *out = new T[nb_bytes];
      return message::rec_sync<T>(src, tag, nb_bytes, *out);
    }

    /**
     * @brief Blocking receive of a message from a particular node.
     *
     * @tparam T Type of element.
     * @param src Source node.
     * @param tag Operation identifier.
     * @param status Status object to retrieve data size.
     * @param nb_bytes Store data size.
     * @param out Data to retrieve.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    rec_sync(int src, int tag, MPI_Status& status, int* nb_bytes, T** out)
    {
      MPI_Get_count(&status, MPI_BYTE, nb_bytes);

      *out = new T[*nb_bytes];
      return message::rec_sync<T>(src, tag, *nb_bytes, *out);
    }

    /**
     * @brief Blocking receive of a message from a particular node.
     *
     * @tparam T Type of element.
     * @param src Source node.
     * @param tag Operation identifier.
     * @param out Data to retrieve.
     *
     * @return MPI error code.
     */
    template <typename T>
    inline int
    rec_sync(int src, int tag, T** out)
    {
      MPI_Status status;
      MPI_Probe(src, tag, MPI_COMM_WORLD, &status);

      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      if (nb_bytes != 0) *out = new T[nb_bytes];

      return message::rec_sync<T>(src, tag, nb_bytes, *out);
    }
  }  // namespace message
}  // namespace algorep
