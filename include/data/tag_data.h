#pragma once

/**
 * @file tag_data.h
 * @brief Define tags sent in messages.
 * @author David Peicho, Sarasvati Moutoucomarapoulé
 * @version 1.0
 * @date 2017-12-21
 */

namespace algorep
{
  /**
   * @brief Operations identifiers.
   */
  enum TAGS
  {
    ALLOCATION = 0,
    READ,
    WRITE,
    FREE,
    MAP,
    REDUCE,
    QUIT
  };
}  // namespace algorep
