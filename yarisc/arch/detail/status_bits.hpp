/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_STATUS_BITS_HPP
#define YARISC_ARCH_DETAIL_STATUS_BITS_HPP

#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <bitset>
#include <cstddef>
#include <ostream>
#include <string_view>
#include <type_traits>

namespace yarisc::arch::detail
{
  [[nodiscard]] inline char named_bit(word_t status, word_t flag, char ch) noexcept
  {
    return (status & flag) ? ch : '0';
  }

  [[nodiscard]] inline char negative_bit(word_t status) noexcept
  {
    return named_bit(status, status_register::negative_flag, 'N');
  }

  [[nodiscard]] inline char zero_bit(word_t status) noexcept
  {
    return named_bit(status, status_register::zero_flag, 'Z');
  }

  [[nodiscard]] inline char carry_bit(word_t status) noexcept
  {
    return named_bit(status, status_register::carry_flag, 'C');
  }

  [[nodiscard]] inline char overflow_bit(word_t status) noexcept
  {
    return named_bit(status, status_register::overflow_flag, 'V');
  }

  class status_bits_t : public bind_output_fn<status_bits_t, word_t>
  {
  public:
    using highlight_diff_only = std::true_type;

    explicit status_bits_t() = default;

  private:
    friend bind_output_fn<status_bits_t, word_t>;

    static constexpr std::size_t bits = 8 * sizeof(word_t);

    template <utils::color::context Ctx>
    void put(word_t status, Ctx& ctx, std::ostream& os) const
    {
      using namespace std::string_view_literals;

      if (status & ~status_register::mask)
      {
        // Display all bits of the status register
        std::string status_bits = std::bitset<bits>{status}.to_string();

        if (status_bits.size() == bits)
        {
          status_bits[bits - status_register::negative_pos - 1] = negative_bit(status); // N
          status_bits[bits - status_register::zero_pos - 1] = zero_bit(status);         // Z
          status_bits[bits - status_register::carry_pos - 1] = carry_bit(status);       // C
          status_bits[bits - status_register::overflow_pos - 1] = overflow_bit(status); // V
        }

        os << "status: "sv;
        os << utils::color::bright_white(ctx) << "0b"sv << status_bits << utils::color::reset(ctx);
      }
      else
      {
        // Display only the known bits of the status register
        os << "              status: "sv;
        os << utils::color::bright_white(ctx) //
           << negative_bit(status)            // N
           << zero_bit(status)                // Z
           << carry_bit(status)               // C
           << overflow_bit(status)            // V
           << utils::color::reset(ctx);
      }
    }
  };

  inline constexpr status_bits_t status_bits{};

} // namespace yarisc::arch::detail

#endif
