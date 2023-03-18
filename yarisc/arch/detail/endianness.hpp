/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_ENDIANNESS_HPP
#define YARISC_ARCH_DETAIL_ENDIANNESS_HPP

#include <yarisc/arch/types.hpp>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace yarisc::arch::detail
{
  static_assert(sizeof(word_t) == 2);

  inline constexpr bool little_endian_v = (std::bit_cast<uint16_t>(std::array<std::uint8_t, 2>{0x34, 0x12}) == 0x1234);

  [[nodiscard]] inline word_t load_word(const std::byte* data) noexcept
  {
    if constexpr (little_endian_v)
    {
      return *reinterpret_cast<const word_t*>(data);
    }
    else
    {
      return (static_cast<word_t>(static_cast<std::uint8_t>(*data)) << 8) +
             static_cast<word_t>(static_cast<std::uint8_t>(*(data + 1)));
    }
  }

  inline void store_word(std::byte* data, word_t value) noexcept
  {
    if constexpr (little_endian_v)
    {
      *reinterpret_cast<word_t*>(data) = value;
    }
    else
    {
      *data = static_cast<std::byte>(static_cast<std::uint8_t>(value >> 8));
      *(data + 1) = static_cast<std::byte>(static_cast<std::uint8_t>(value & 0xff));
    }
  }

} // namespace yarisc::arch::detail

#endif
