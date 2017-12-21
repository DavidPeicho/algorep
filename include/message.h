#pragma once

#include <vector>

#include <mpi/mpi.h>

/**
 * @file message.h
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace message
  {
    /**
     * @brief 
     *
     * @tparam T
     * @param buffer
     * @param nb_bytes
     * @param dest
     * @param tag
     * @param request
     *
     * @return 
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
     * @brief 
     *
     * @param str
     * @param dest
     * @param tag
     * @param request
     *
     * @return 
     */
    inline int
    send(const std::string& str, int dest, int tag, MPI_Request& request)
    {
      const char* buffer = str.c_str();
      size_t nb_bytes = str.length() + 1;  // Do not forget the '\0'

      return send<char>(buffer, nb_bytes, dest, tag, request);
    }

    /**
     * @brief 
     *
     * @tparam T
     * @param buffer
     * @param nb_bytes
     * @param dest
     * @param tag
     *
     * @return 
     */
    template <typename T>
    inline int
    send_sync(const T* buffer, size_t nb_bytes, int dest, int tag)
    {
      return MPI_Send(buffer, nb_bytes, MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    }

    /**
     * @brief 
     *
     * @param str
     * @param dest
     * @param tag
     *
     * @return 
     */
    inline int
    send_sync(const std::string& str, int dest, int tag)
    {
      const char* buffer = str.c_str();
      size_t nb_bytes = str.length() + 1;  // Do not forget the '\0'
      return send_sync<char>(buffer, nb_bytes, dest, tag);
      // return MPI_Send(buffer, nb_bytes, MPI_BYTE, dest, tag, MPI_COMM_WORLD);
    }

    /**
     * @brief 
     *
     * @param dest
     * @param tag
     * @param out
     *
     * @return 
     */
    inline int
    rec_sync_ack(int dest, int tag, uint8_t& out)
    {
      return MPI_Recv(&out, 1, MPI_BYTE, dest, tag, MPI_COMM_WORLD,
                      MPI_STATUS_IGNORE);
    }

    /**
     * @brief 
     *
     * @tparam T
     * @param dest
     * @param tag
     * @param bytes
     * @param out
     *
     * @return 
     */
    template <typename T>
    inline int
    rec_sync(int dest, int tag, int bytes, T* out)
    {
      return MPI_Recv(out, bytes, MPI_BYTE, dest, tag, MPI_COMM_WORLD,
                      MPI_STATUS_IGNORE);
    }

    /**
     * @brief 
     *
     * @tparam T
     * @param dest
     * @param tag
     * @param status
     * @param out
     *
     * @return 
     */
    template <typename T>
    inline int
    rec_sync(int dest, int tag, MPI_Status& status, T** out)
    {
      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      *out = new T[nb_bytes];
      return message::rec_sync<T>(dest, tag, nb_bytes, *out);
    }

    /**
     * @brief 
     *
     * @tparam T
     * @param dest
     * @param tag
     * @param status
     * @param nb_bytes
     * @param out
     *
     * @return 
     */
    template <typename T>
    inline int
    rec_sync(int dest, int tag, MPI_Status& status, int* nb_bytes, T** out)
    {
      MPI_Get_count(&status, MPI_BYTE, nb_bytes);

      *out = new T[*nb_bytes];
      return message::rec_sync<T>(dest, tag, *nb_bytes, *out);
    }

    /**
     * @brief 
     *
     * @tparam T
     * @param dest
     * @param tag
     * @param out
     *
     * @return 
     */
    template <typename T>
    inline int
    rec_sync(int dest, int tag, T** out)
    {
      MPI_Status status;
      MPI_Probe(dest, tag, MPI_COMM_WORLD, &status);

      int nb_bytes = 0;
      MPI_Get_count(&status, MPI_BYTE, &nb_bytes);

      if (nb_bytes != 0) *out = new T[nb_bytes];

      return message::rec_sync<T>(dest, tag, nb_bytes, *out);
    }
  }  // namespace message
}  // namespace algorep
