/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_HEX_WORD_HPP
#define YARISC_ARCH_DETAIL_HEX_WORD_HPP

#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <concepts>
#include <iomanip>
#include <ostream>
#include <string_view>
#include <type_traits>

namespace yarisc::arch::detail
{
  template <std::integral T>
  std::ostream& output_hex(std::ostream& os, T value, std::streamsize width = 2 * sizeof(T))
  {
    return os << std::setfill('0') << std::setw(width) << std::hex << value;
  }

  template <std::integral T>
    requires std::is_unsigned_v<T>
  class hex_integral_t : public bind_output_fn<hex_integral_t<T>, T>
  {
  public:
    explicit hex_integral_t() = default;

  private:
    friend bind_output_fn<hex_integral_t, T>;

    template <utils::color::context Ctx>
    void put(T value, Ctx& ctx, std::ostream& os) const
    {
      using namespace std::string_view_literals;

      output_hex(os << utils::color::bright_white(ctx) << "0x"sv, value) << utils::color::reset(ctx);
    }
  };

  template <typename T>
  class hex_sequence_t : public bind_output_fn<hex_sequence_t<T>, T>
  {
  public:
    explicit hex_sequence_t() = default;

  private:
    friend bind_output_fn<hex_sequence_t, T>;

    static constexpr std::streamsize width = 2 * sizeof(typename T::value_type);

    template <utils::color::context Ctx>
    void put(const T& value, Ctx& ctx, std::ostream& os) const
    {
      if (!value.empty())
      {
        const auto color_a = utils::color::bright_white(ctx);
        const auto color_b = utils::color::white(ctx);

        auto first = begin(value);
        const auto last = end(value);

        output_hex(os << color_a, static_cast<word_t>(*(first++)), width);

        typename T::size_type n = 0;

        for (; first != last; ++first)
          output_hex(os << ' ' << ((++n & 1) ? color_b : color_a), static_cast<word_t>(*first), width);

        os << utils::color::reset(ctx);
      }
    }
  };

  inline constexpr hex_integral_t<word_t> hex_word{};
  inline constexpr hex_sequence_t<word_as_bytes_t> hex_word_as_bytes{};

} // namespace yarisc::arch::detail

#endif
