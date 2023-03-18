/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_FEATURE_LEVEL_HPP
#define YARISC_ARCH_FEATURE_LEVEL_HPP

#include <cstdint>

namespace yarisc::arch
{
  namespace detail
  {
    using feature_level_t = std::uint16_t;

  } // namespace detail

  /**
   * @brief Feature level
   */
  enum class feature_level : detail::feature_level_t
  {
    none = 0,

    /**
     * @brief Minimalistic feature set that is simple to build in hardware (YaRISC-min)
     */
    min = 10,

    /**
     * @brief The first very basic version (YaRISC-1)
     */
    v1 = 100,
  };

  /**
   * @brief The latest feature level
   */
  inline constexpr feature_level feature_level_latest = feature_level::v1;

} // namespace yarisc::arch

#endif
