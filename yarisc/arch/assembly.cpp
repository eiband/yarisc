/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/arch/assembly.hpp>

#include <yarisc/arch/detail/hex_word.hpp>
#include <yarisc/arch/registers.hpp>

#include <array>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace yarisc::arch
{
  namespace
  {
    static_assert(num_registers == 8);

    constinit const std::string_view mnemonic_sep = " ";
    constinit const std::string_view argument_sep = ", ";

    constinit const std::array<std::string_view, num_registers> reg_names{
      {"r0", "r1", "r2", "r3", "r4", "r5", "sp", "ip"}};

    [[nodiscard]] disassembly invalid_level_error(feature_level level)
    {
      return {0, "Invalid feature level " + std::to_string(static_cast<std::underlying_type_t<feature_level>>(level))};
    }

    [[nodiscard]] disassembly invalid_opcode_error(word_t instr)
    {
      using namespace std::string_view_literals;

      std::ostringstream oss;
      detail::output_hex(oss << "Invalid instruction 0x"sv, instr);

      return {0, std::move(oss).str()};
    }

    [[nodiscard]] disassembly invalid_bits_error(word_t instr)
    {
      using namespace std::string_view_literals;

      std::ostringstream oss;
      detail::output_hex(oss << "Invalid non-zero bits in instruction 0x"sv, instr);

      return {0, std::move(oss).str()};
    }

    [[nodiscard]] bool check_no_operands(word_t instr) noexcept
    {
      return !(instr & operand_mask);
    }

    [[nodiscard]] bool check_one_operand(word_t instr) noexcept
    {
      return !(instr & (operand_op1_mask | operand_op2_mask));
    }

    [[nodiscard]] bool check_two_operands(word_t instr) noexcept
    {
      return (instr & operand_sel_mask)
               ? !(instr & operand_as_mask) && !((instr & operand_loc_mask) && (instr & operand_st_mask))
               : !(instr & operand_op2_mask);
    }

    [[nodiscard]] bool check_three_operands(word_t instr) noexcept
    {
      return !((instr & operand_imm_invalid_mask) == operand_imm_invalid_mask);
    }

    [[nodiscard]] bool check_jump(word_t instr) noexcept
    {
      return !((instr & operand_addr_loc_mask) && (instr & operand_addr_mask));
    }

    [[nodiscard]] bool check_cond_jump(word_t instr) noexcept
    {
      return !((instr & operand_addr_loc_mask) && (instr & operand_cond_addr_mask)) &&
             !((instr & operand_cond_invalid_mask) == operand_cond_invalid_mask);
    }

    std::ostream& output_first_reg_operand(std::ostream& os, word_t instr)
    {
      return os << reg_names[(instr & operand_op0_mask) >> operand_op0_offset];
    }

    std::ostream& output_second_reg_operand(std::ostream& os, word_t instr)
    {
      return os << reg_names[(instr & operand_op1_mask) >> operand_op1_offset];
    }

    std::ostream& output_third_reg_operand(std::ostream& os, word_t instr)
    {
      return os << reg_names[(instr & operand_op2_mask) >> operand_op2_offset];
    }

    std::ostream& output_immediate_impl(std::ostream& os, word_t imm)
    {
      using namespace std::string_view_literals;

      const auto width = [](word_t imm)
      { return (imm < 16) ? 1 : ((imm < (1 << (8 * (sizeof(word_t) - 1)))) ? sizeof(word_t) : 2 * sizeof(word_t)); };

      return (imm < 10) ? (os << imm) : detail::output_hex(os << "0x"sv, imm, width(imm));
    }

    std::ostream& output_immediate(std::ostream& os, word_t imm, int& words)
    {
      ++words;

      return output_immediate_impl(os, imm);
    }

    std::ostream& output_short_immediate(std::ostream& os, word_t instr)
    {
      const auto imm = static_cast<word_t>((instr & operand_st_mask) >> operand_st_offset);

      return output_immediate_impl(os, imm);
    }

    std::ostream& output_address_impl(std::ostream& os, address_t address)
    {
      using namespace std::string_view_literals;

      return detail::output_hex(os << "0x"sv, address);
    }

    std::ostream& output_short_jump_address(std::ostream& os, word_t instr)
    {
      static_assert(sizeof(word_t) == 2);

      // Adjust offset to account for word address
      const auto address = static_cast<address_t>((instr & operand_addr_mask) >> (operand_addr_offset - 1));

      return output_address_impl(os, address);
    }

    std::ostream& output_short_cond_jump_address(std::ostream& os, word_t instr)
    {
      static_assert(sizeof(word_t) == 2);

      // Adjust offset to account for word address
      const auto address = static_cast<address_t>((instr & operand_cond_addr_mask) >> (operand_cond_addr_offset - 1));

      return output_address_impl(os, address);
    }

    [[nodiscard]] disassembly convert_one_operand(std::string_view mnemonic, word_t instr)
    {
      std::ostringstream oss;
      output_first_reg_operand(oss << mnemonic << mnemonic_sep, instr);

      return {1, std::move(oss).str()};
    }

    [[nodiscard]] disassembly convert_two_operands(std::string_view mnemonic, word_t instr, word_t arg)
    {
      int words = 1;

      std::ostringstream oss;
      output_first_reg_operand(oss << mnemonic << mnemonic_sep, instr) << argument_sep;

      if (instr & operand_sel_mask)
        (instr & operand_loc_mask) ? output_immediate(oss, arg, words) : output_short_immediate(oss, instr);
      else
        output_second_reg_operand(oss, instr);

      return {words, std::move(oss).str()};
    }

    [[nodiscard]] disassembly convert_three_operands(std::string_view mnemonic, word_t instr, word_t arg)
    {
      int words = 1;

      std::ostringstream oss;
      output_first_reg_operand(oss << mnemonic << mnemonic_sep, instr) << argument_sep;

      if (instr & operand_sel_mask)
      {
        const auto get_first_reg_name = [](word_t instr)
        { return std::string{reg_names[(instr & operand_op0_mask) >> operand_op0_offset]}; };
        const auto get_second_reg_name = [](word_t instr)
        { return std::string{reg_names[(instr & operand_op1_mask) >> operand_op1_offset]}; };

        const auto get_immediate = [](word_t arg, int& words)
        {
          std::ostringstream oss;
          output_immediate(oss, arg, words);

          return std::move(oss).str();
        };

        const auto get_short_immediate = [](word_t instr)
        {
          std::ostringstream oss;
          output_short_immediate(oss, instr);

          return std::move(oss).str();
        };

        const auto operands = (instr & operand_loc_mask)
                                ? std::array<std::string, 2>{{get_immediate(arg, words), get_second_reg_name(instr)}}
                                : std::array<std::string, 2>{{get_short_immediate(instr), get_first_reg_name(instr)}};

        const auto as = static_cast<unsigned int>((instr & operand_as_mask) >> operand_as_offset);

        oss << operands[as] << argument_sep << operands[static_cast<unsigned int>(1 - as)];
      }
      else
      {
        output_second_reg_operand(oss, instr) << argument_sep;
        output_third_reg_operand(oss, instr);
      }

      return {words, std::move(oss).str()};
    }

    [[nodiscard]] disassembly convert_jump_operand(std::string_view mnemonic, word_t instr, word_t arg)
    {
      int words = 1;

      std::ostringstream oss;
      oss << mnemonic << mnemonic_sep;

      (instr & operand_addr_loc_mask) ? output_immediate(oss, arg, words) : output_short_jump_address(oss, instr);

      return {words, std::move(oss).str()};
    }

    [[nodiscard]] disassembly convert_cond_jump_operand(std::string_view mnemonic, word_t instr, word_t arg)
    {
      int words = 1;

      std::ostringstream oss;
      oss << mnemonic << ((instr & operand_cond_neg_mask) ? 'N' : 'M');

      if (instr & operand_cond_flag_carry_mask)
        oss << 'C';
      if (instr & operand_cond_flag_zero_mask)
        oss << 'Z';

      oss << mnemonic_sep;

      (instr & operand_addr_loc_mask) ? output_immediate(oss, arg, words) : output_short_cond_jump_address(oss, instr);

      return {words, std::move(oss).str()};
    }

    template <optype Type>
    struct disassemble_traits;

    template <>
    struct disassemble_traits<optype::basic>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t)
      {
        return check_no_operands(instr) ? disassembly{1, std::string{mnemonic}} : invalid_bits_error(instr);
      }
    };

    template <>
    struct disassemble_traits<optype::op0>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t)
      {
        return check_one_operand(instr) ? convert_one_operand(mnemonic, instr) : invalid_bits_error(instr);
      }
    };

    template <>
    struct disassemble_traits<optype::op0_op1>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t arg)
      {
        return check_two_operands(instr) ? convert_two_operands(mnemonic, instr, arg) : invalid_bits_error(instr);
      }
    };

    template <>
    struct disassemble_traits<optype::op0_op1_op2>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t arg)
      {
        return check_three_operands(instr) ? convert_three_operands(mnemonic, instr, arg) : invalid_bits_error(instr);
      }
    };

    template <>
    struct disassemble_traits<optype::jump>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t arg)
      {
        return check_jump(instr) ? convert_jump_operand(mnemonic, instr, arg) : invalid_bits_error(instr);
      }
    };

    template <>
    struct disassemble_traits<optype::cond_jump>
    {
      [[nodiscard]] static disassembly disassemble(std::string_view mnemonic, word_t instr, word_t arg)
      {
        return check_cond_jump(instr) ? convert_cond_jump_operand(mnemonic, instr, arg) : invalid_bits_error(instr);
      }
    };

    template <opcode Code, typename Profile>
    [[nodiscard]] disassembly disassemble_opcode(word_t instr, [[maybe_unused]] word_t arg)
    {
      using profile_type = Profile;
      using traits_type = disassemble_traits<profile_type::template instruction_type<Code>>;

      if constexpr (profile_type::template instruction_supported<Code>)
        return traits_type::disassemble(profile_type::template instruction_mnemonic<Code>, instr, arg);
      else
        return invalid_opcode_error(instr);
    }

    template <typename Profile>
    [[nodiscard]] disassembly disassemble_instruction(word_t instr, word_t arg)
    {
      switch (static_cast<opcode>(instr & opcode_mask))
      {
      case opcode::move:
        return disassemble_opcode<opcode::move, Profile>(instr, arg);
      case opcode::load:
        return disassemble_opcode<opcode::load, Profile>(instr, arg);
      case opcode::store:
        return disassemble_opcode<opcode::store, Profile>(instr, arg);
      case opcode::add:
        return disassemble_opcode<opcode::add, Profile>(instr, arg);
      case opcode::add_with_carry:
        return disassemble_opcode<opcode::add_with_carry, Profile>(instr, arg);
      case opcode::jump:
        return disassemble_opcode<opcode::jump, Profile>(instr, arg);
      case opcode::cond_jump:
        return disassemble_opcode<opcode::cond_jump, Profile>(instr, arg);
      case opcode::noop:
        return disassemble_opcode<opcode::noop, Profile>(instr, arg);
      case opcode::halt:
        return disassemble_opcode<opcode::halt, Profile>(instr, arg);
      default:
        return invalid_opcode_error(instr);
      }
    }

  } // namespace

  namespace detail
  {
    void throw_invalid_immediate(word_t imm, word_t mask)
    {
      using namespace std::string_view_literals;

      std::ostringstream oss;
      output_hex(oss << "short immediate constant "sv, imm) << "too large"sv;
      output_hex(oss << " (mask: "sv, mask) << ")"sv;

      throw std::out_of_range{std::move(oss).str()};
    }

  } // namespace detail

  disassembly disassemble(word_t instr, word_t arg, feature_level level)
  {
    switch (level)
    {
    case feature_level::min:
      return disassemble_instruction<machine_profile<feature_level::min>>(instr, arg);
    case feature_level::v1:
      return disassemble_instruction<machine_profile<feature_level::v1>>(instr, arg);
    default:
      return invalid_level_error(level);
    }
  }

} // namespace yarisc::arch
