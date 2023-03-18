/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_HEX_MEMORY_HPP
#define YARISC_ARCH_DETAIL_HEX_MEMORY_HPP

#include <yarisc/arch/detail/colors.hpp>
#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/detail/hex_word.hpp>
#include <yarisc/arch/machine.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <string_view>

namespace yarisc::arch::detail
{
  template <std::size_t N, utils::color::context Ctx>
    requires(N < num_registers)
  [[nodiscard]] bool address_register_bgclr(std::ostream& os, Ctx& ctx, address_t address, const machine_state& state)
  {
    if constexpr (register_background_colors[N].second)
    {
      if (address == state.reg.named.r[N])
      {
        os << utils::color::manip(ctx, register_background_colors[N].first);

        return true;
      }
    }

    return false;
  }

  template <utils::color::context Ctx>
  std::ostream& output_address_bgclr(std::ostream& os, Ctx& ctx, address_t address, const machine_state& state)
  {
    static_assert(num_registers == 8);

    // Instruction pointer takes precedence
    if (address_register_bgclr<7>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<0>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<1>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<2>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<3>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<4>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<5>(os, ctx, address, state))
      return os;
    if (address_register_bgclr<6>(os, ctx, address, state))
      return os;

    return os;
  }

  template <typename T, utils::color::context Ctx>
  std::ostream& output_word_as_bytes(
    std::ostream& os, const T& word, Ctx& ctx, address_t address, const machine_state* state)
  {
    using namespace std::string_view_literals;

    if (state)
      output_address_bgclr(os, ctx, address, *state);

    return os << hex_word_as_bytes(word, ctx) << "  "sv;
  }

  template <memory_view::size_type Width>
  class hex_memory_t : public bind_output_fn<hex_memory_t<Width>, memory_view>
  {
  public:
    explicit hex_memory_t() = default;

  private:
    friend bind_output_fn<hex_memory_t, memory_view>;

    using size_type = memory_view::size_type;

    // See loops in put_line()
    static_assert(sizeof(word_as_bytes_t::value_type) == 1);
    static_assert(is_aligned(word_as_bytes_t{}.size()));

    static constexpr size_type width_bytes = Width * sizeof(word_t);

    using char_line_buffer_type = std::array<char, width_bytes>;

    template <utils::color::context Ctx>
    void put(memory_view view, Ctx& ctx, std::ostream& os) const
    {
      while (!view.empty())
        view = view.sub(put_line(view.sub(0, width_bytes), ctx, os));
    }

    template <utils::color::context Ctx>
    void put(const diff<memory_view>& d, Ctx& ctx, std::ostream& os) const
    {
      if constexpr (utils::color::always_enabled<Ctx>)
        put(d.current, d.previous, ctx, os);
      else if constexpr (utils::color::never_enabled<Ctx>)
        put(d.current, ctx, os);
      else if (ctx.enabled())
        put(d.current, d.previous, ctx, os);
      else
        put(d.current, ctx, os);
    }

    template <utils::color::context Ctx>
    void put(memory_view current, memory_view previous, Ctx& ctx, std::ostream& os) const
    {
      previous = adjust_previous(current, previous);

      if (previous.empty())
      {
        // No overlap between previous and current memory view
        put(current, ctx, os);
      }
      else
      {
        while (!current.empty())
        {
          const auto offset = put_line(current.sub(0, width_bytes), previous, ctx, os);

          current = current.sub(offset);
        }
      }
    }

    template <utils::color::context Ctx>
    size_type put_line(memory_view line, Ctx& ctx, std::ostream& os) const
    {
      const address_t base = line.base();
      const machine_state* const state = line.state();

      put_address(base, os);

      assert(is_aligned(line.size()));

      word_as_bytes_t word;
      char_line_buffer_type buf;

      for (size_type i = 0; i < line.size(); i += word.size())
      {
        for (size_type j = 0; j < word.size(); ++j)
        {
          const size_type k = i + j;

          buf[k] = static_cast<char>(word[j] = line[k]);
        }

        output_word_as_bytes(os, word, ctx, base + static_cast<address_t>(i), state);
      }

      const std::string_view chars{buf.data(), line.size()};

      os << string_format(chars, ctx) << '\n';

      return line.size();
    }

    template <utils::color::context Ctx>
    size_type put_line(memory_view current, memory_view previous, Ctx& ctx, std::ostream& os) const
    {
      previous = adjust_previous(current, previous);

      if (previous.empty())
        return put_line(current, ctx, os);

      const address_t base = current.base();
      const machine_state* const state = current.state();

      put_address(current.base(), os);

      assert(is_aligned(current.size()));
      assert(is_aligned(previous.size()));

      size_type i = 0;
      word_as_bytes_t current_word, previous_word;
      char_line_buffer_type current_buf, previous_buf;

      assert(current.base() <= previous.base());

      const size_type previous_offset = static_cast<size_type>(previous.base() - current.base());

      assert(previous_offset <= current.size());

      for (; i < previous_offset; i += current_word.size())
      {
        for (size_type j = 0; j < current_word.size(); ++j)
        {
          const size_type k = i + j;

          current_buf[k] = previous_buf[k] = static_cast<char>(current_word[j] = current[k]);
        }

        output_word_as_bytes(os, current_word, ctx, base + static_cast<address_t>(i), state);
      }

      const size_type previous_last = previous_offset + previous.size();

      assert(previous_last <= current.size());

      for (; i < previous_last; i += current_word.size())
      {
        for (size_type j = 0; j < current_word.size(); ++j)
        {
          const size_type k = i + j;

          current_buf[k] = static_cast<char>(current_word[j] = current[k]);
          previous_buf[k] = static_cast<char>(previous_word[j] = previous[k - previous_offset]);
        }

        output_word_as_bytes(os, diff(current_word, previous_word), ctx, base + static_cast<address_t>(i), state);
      }

      for (; i < current.size(); i += current_word.size())
      {
        for (size_type j = 0; j < current_word.size(); ++j)
        {
          const size_type k = i + j;

          current_buf[k] = previous_buf[k] = static_cast<char>(current_word[j] = current[k]);
        }

        output_word_as_bytes(os, current_word, ctx, base + static_cast<address_t>(i), state);
      }

      const std::string_view current_chars{current_buf.data(), current.size()};
      const std::string_view previous_chars{previous_buf.data(), current.size()};

      os << string_format(diff(current_chars, previous_chars), ctx) << '\n';

      return current.size();
    }

    void put_address(address_t address, std::ostream& os) const
    {
      using namespace std::string_view_literals;

      utils::color::plain_context ctx{};

      os << hex_word(address, ctx) << ": "sv;
    }

    [[nodiscard]] static memory_view adjust_previous(memory_view current, memory_view previous) noexcept
    {
      static_assert(sizeof(size_type) > sizeof(address_t));

      const size_type current_first = current.base();
      const size_type current_last = current_first + current.size();

      const size_type previous_first = previous.base();
      const size_type previous_last = previous_first + previous.size();

      const size_type overlap_first = std::max(current_first, previous_first);
      const size_type overlap_last = std::min(current_last, previous_last);

      if (overlap_first < overlap_last)
      {
        // Compute overlap of previous with current
        const size_type current_offset = static_cast<size_type>(overlap_first - current_first);
        const size_type previous_offset = static_cast<size_type>(overlap_first - previous_first);
        const size_type overlap_size = static_cast<size_type>(overlap_last - overlap_first);

        const auto* const current_data = current.data() + current_offset;
        const auto* const previous_data = previous.data() + previous_offset;

        if (!std::equal(current_data, current_data + overlap_size, previous_data))
          return previous.sub(previous_offset, overlap_size);
      }

      return {};
    }
  };

  inline constexpr hex_memory_t<8> hex_memory{};

} // namespace yarisc::arch::detail

#endif
