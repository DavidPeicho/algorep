#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file memory.h
 * @brief 
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace
  {
    /**
     * @brief Contains the clock of a message, as well as its size.
     */
    using Pack = std::tuple<size_t, int>;
  }

  /**
   * @brief 
   */
  class Memory
  {
    public:
    /**
     * @brief Allocate space in memory for some data.
     *
     * @param node_rank Rank of the node where is allocated the data.
     * @param nb_bytes Number of bytes requested.
     *
     * @return Data identifier.
     */
    std::string
    reserve(int node_rank, int nb_bytes);

    /**
     * @brief Release all data.
     */
    void
    release();

    /**
     * @brief Release specific data.
     *
     * @param id Data to release.
     */
    void
    release(const std::string& id);

    public:
    /**
     * @brief Get specific data.
     *
     * @param id Data to get.
     *
     * @return Data queried.
     */
    inline std::vector<uint8_t>&
    get(const std::string& id)
    {
      return this->data_[id];
    }

    /**
     * @brief Get specific data as const.
     *
     * @param id Data to get.
     *
     * @return Data queried.
     */
    inline const std::vector<uint8_t>&
    getConst(const std::string& id) const
    {
      return this->data_.at(id);
    }

    /**
     * @brief 
     *
     * @return 
     */
    inline std::unordered_map<std::string, std::tuple<Pack, Pack>>&
    history()
    {
      return this->history_;
    }

    private:
    /**
     * @brief Store data associated with its identifier.
     */
    std::unordered_map<std::string, std::vector<uint8_t>> data_;

    /**
     * @brief 
     */
    std::unordered_map<std::string, std::tuple<Pack, Pack>> history_;
  };
}  // namespace algorep
