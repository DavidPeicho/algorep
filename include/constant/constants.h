#pragma once

/**
 * @file constants.h
 * @brief Define constants used in the project.
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  namespace constant
  {
    /**
     * @brief Maximum size for identifier.
     */
    constexpr static unsigned int ID_LEN = 23;

    /**
     * @brief Code sent in case of failure.
     */
    constexpr static uint8_t FAIL = 0;

    /**
     * @brief Code sent in case of success.
     */
    constexpr static uint8_t SUCCESS = 1;
  }  // namespace constant
}  // namespace algorep
