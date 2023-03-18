/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_COLORS_HPP
#define YARISC_ARCH_DETAIL_COLORS_HPP

#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <array>
#include <cstddef>
#include <iosfwd>
#include <string_view>
#include <utility>

namespace yarisc::arch::detail
{
  struct color_noop
  {
  };

  inline std::ostream& operator<<(std::ostream& os, color_noop) noexcept
  {
    return os;
  }

  inline constexpr std::array<std::pair<std::string_view, bool>, num_registers> register_background_colors{{
    {utils::color::blue_background_seq, false},           // r0
    {utils::color::magenta_background_seq, false},        // r1
    {utils::color::cyan_background_seq, false},           // r2
    {utils::color::bright_blue_background_seq, false},    // r3
    {utils::color::bright_magenta_background_seq, false}, // r4
    {utils::color::bright_cyan_background_seq, false},    // r5
    {utils::color::yellow_background_seq, true},          // sp
    {utils::color::green_background_seq, true},           // ip
  }};

  template <std::size_t N, utils::color::context Ctx>
    requires(N < num_registers)
  [[nodiscard]] auto register_bgclr(Ctx& ctx) noexcept
  {
    if constexpr (register_background_colors[N].second)
      return utils::color::manip(ctx, register_background_colors[N].first);
    else
      return color_noop{};
  }

} // namespace yarisc::arch::detail

#endif
