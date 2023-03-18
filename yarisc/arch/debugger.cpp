/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/arch/debugger.hpp>

#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/utils/color.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace yarisc::arch
{
  namespace
  {
    template <typename>
    inline constexpr bool always_false_v = false;

    std::ostream& output_message(std::ostream& os, std::string_view msg)
    {
      constexpr std::size_t line_width = 80;

      std::array<char, line_width + 1> line;
      line[line_width] = '\n';

      // Trim the message
      if (!msg.empty())
        msg = msg.substr(0, line_width);

      std::ranges::fill(std::ranges::copy(msg, begin(line)).out, end(line) - 1, ' ');

      return os << std::string_view{line.data(), line.size()};
    }

    class debugger_output_t : public detail::bind_output_fn<debugger_output_t, debugger_view>
    {
    public:
      explicit debugger_output_t() = default;

    private:
      friend detail::bind_output_fn<debugger_output_t, debugger_view>;

      template <utils::color::context Ctx>
      void put(const debugger_view& dbg, Ctx& ctx, std::ostream& os) const
      {
        using namespace std::string_view_literals;

        // We don't color the diff of the instruction pointer because it changes almost always
        registers_view previous_registers_except_ip = dbg.previous_registers;
        previous_registers_except_ip.named.set_ip(dbg.current_registers.named.ip());
        previous_registers_except_ip.instruction = dbg.current_registers.instruction;

        // Output registers
        os << as_diff(dbg.current_registers, previous_registers_except_ip) << '\n';

        // Output main memory
        os << as_diff(dbg.current_memory, dbg.previous_memory) << '\n';

        // Output info or error (always output a message to clear previous messages)
        if (!dbg.error.empty())
          output_message(os << utils::color::bright_red(ctx), dbg.error) << utils::color::reset(ctx);
        else
          output_message(os, dbg.info);
      }

      template <utils::color::context Ctx>
      void put(const diff<debugger_view>&, Ctx&, std::ostream&) const
      {
        static_assert(always_false_v<Ctx>, "Debugger view already outputs a diff");
      }
    };

    inline constexpr debugger_output_t debugger_output{};

  } // namespace

  bool store_debug_message(debugger* dbg, std::string msg)
  {
    if (!dbg)
      return false;

    dbg->message_ = std::move(msg);

    return true;
  }

  void store_panic_or_throw(debugger* dbg, std::string msg)
  {
    if (!dbg)
      throw_panic(msg);

    dbg->panic_ = true;
    dbg->message_ = std::move(msg);
  }

  void throw_panic(const std::string& msg)
  {
    throw std::runtime_error{msg};
  }

  void tag_invoke(output_t, std::ostream& os, const debugger_view& dbg, output_format fmt)
  {
    detail::format_output(debugger_output, os, fmt, dbg);
  }

} // namespace yarisc::arch
