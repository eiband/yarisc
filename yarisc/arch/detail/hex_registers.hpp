/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_HEX_REGISTERS_HPP
#define YARISC_ARCH_DETAIL_HEX_REGISTERS_HPP

#include <yarisc/arch/assembly.hpp>
#include <yarisc/arch/detail/colors.hpp>
#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/detail/hex_word.hpp>
#include <yarisc/arch/detail/status_bits.hpp>
#include <yarisc/arch/machine_model.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <cassert>
#include <iostream>
#include <string_view>

namespace yarisc::arch::detail
{
  class hex_registers_t : public bind_output_fn<hex_registers_t, registers_view>
  {
  public:
    explicit hex_registers_t() = default;

  private:
    friend bind_output_fn<hex_registers_t, registers_view>;

    template <projectable<registers_view> T, utils::color::context Ctx>
    void put(const T& reg, Ctx& ctx, std::ostream& os) const
    {
      using namespace std::string_view_literals;

      constexpr auto sep = ", "sv;
      constexpr auto space = "                              "sv;

      constexpr auto registers_preamble = "Register: "sv;
      constexpr auto instruction_preamble = "Next:     "sv;

      static_assert(num_registers == 8);

      const auto& named = proj(&registers_view::named, reg);

      os << registers_preamble;
      os << "r0: "sv << register_bgclr<0>(ctx) << hex_word(proj(&registers::r0, named), ctx) << sep // general purpose
         << "r1: "sv << register_bgclr<1>(ctx) << hex_word(proj(&registers::r1, named), ctx) << sep // general purpose
         << "r2: "sv << register_bgclr<2>(ctx) << hex_word(proj(&registers::r2, named), ctx) << sep // general purpose
         << "r3: "sv << register_bgclr<2>(ctx) << hex_word(proj(&registers::r3, named), ctx) << sep // scratch purpose
         << "r4: "sv << register_bgclr<4>(ctx) << hex_word(proj(&registers::r4, named), ctx) << sep // result pointer
         << "r5: "sv << register_bgclr<5>(ctx) << hex_word(proj(&registers::r5, named), ctx)        // return pointer
         << '\n';

      disassembly result{};
      std::string_view current_instruction = space;

      if (const auto& instruction = proj_current(&registers_view::instruction, reg); instruction.has_value())
      {
        result = disassemble((*instruction)[0], (*instruction)[1]);

        if (result.words > 0)
        {
          result.text.reserve(std::max(space.size(), result.text.size() + instruction_preamble.size()));
          result.text.insert(0, instruction_preamble);

          if (result.text.size() > space.size())
            result.text.resize(space.size());
          else if (result.text.size() < space.size())
            result.text.append(space.substr(result.text.size(), space.size() - result.text.size()));

          current_instruction = result.text;
        }
      }

      assert(current_instruction.size() == space.size());

      const auto& status = proj(&registers_view::status, reg);

      os << current_instruction << status_bits(proj(&status_register::s, status), ctx) << sep;
      os << "sp: "sv << register_bgclr<6>(ctx) << hex_word(proj(&registers::sp, named), ctx) << sep // stack pointer
         << "ip: "sv << register_bgclr<7>(ctx) << hex_word(proj(&registers::ip, named), ctx) // instruction pointer
         << '\n';
    }
  };

  inline constexpr hex_registers_t hex_registers{};

} // namespace yarisc::arch::detail

#endif
