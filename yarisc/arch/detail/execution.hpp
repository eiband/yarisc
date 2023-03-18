/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_EXECUTION_HPP
#define YARISC_ARCH_DETAIL_EXECUTION_HPP

#include <yarisc/arch/debugger.hpp>
#include <yarisc/arch/detail/hex_word.hpp>
#include <yarisc/arch/instructions.hpp>
#include <yarisc/arch/machine_model.hpp>
#include <yarisc/arch/machine_profile.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/arch/types.hpp>

#include <array>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace yarisc::arch::detail
{
  struct execute_result final
  {
    bool keep_going{true};
    bool breakpoint{false};
  };

  inline constexpr execute_result halt_result{false, false};
  inline constexpr execute_result breakpoint_result{false, true};

  struct alu_add_op final
  {
    [[nodiscard]] double_word_t operator()(double_word_t op1, double_word_t op2, word_t) const noexcept
    {
      return op1 + op2;
    }
  };

  struct alu_add_with_carry_op final
  {
    [[nodiscard]] double_word_t operator()(double_word_t op1, double_word_t op2, double_word_t carry) const noexcept
    {
      return op1 + op2 + carry;
    }
  };

  template <typename Op>
  struct exec_alu_op
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1, word_t op2) noexcept
    {
      static_assert(status_register::carry_flag == 0x1);

      const auto result = Op{}(op1, op2, reg.status.s & status_register::carry_flag);
      const auto result_word = static_cast<word_t>(result);

      reg.status.s = (result_word == 0x0) ? status_register::zero_flag : 0x0;

      using result_type = std::decay_t<decltype(result)>;

      if constexpr (sizeof(result_type) > sizeof(word_t))
      {
        constexpr auto carry_bit_offset = 8 * sizeof(word_t);
        constexpr auto carry_bit_mask = static_cast<result_type>(1 << carry_bit_offset);

        reg.status.s |= static_cast<word_t>((result & carry_bit_mask) >> carry_bit_offset);
      }

      op0 = result_word;

      return {};
    }
  };

  template <opcode Code>
  struct exec_op;

  template <>
  struct exec_op<opcode::add> : exec_alu_op<alu_add_op>
  {
  };

  template <>
  struct exec_op<opcode::add_with_carry> : exec_alu_op<alu_add_with_carry_op>
  {
  };

  template <>
  struct exec_op<opcode::move>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1) noexcept
    {
      op0 = op1;

      // Update the zero flag
      reg.status.s = (reg.status.s & ~status_register::zero_flag) | ((op1 == 0x0) ? status_register::zero_flag : 0x0);

      return {};
    }
  };

  template <>
  struct exec_op<opcode::load>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, machine_registers&, machine_memory& mem, word_t& op0, word_t op1)
    {
      return policy.load(mem, static_cast<address_t>(op1), op0);
    }
  };

  template <>
  struct exec_op<opcode::store>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, machine_registers&, machine_memory& mem, word_t& op0, word_t op1)
    {
      return policy.store(mem, static_cast<address_t>(op1), op0);
    }
  };

  template <>
  struct exec_op<opcode::jump>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, address_t address) noexcept
    {
      reg.named.set_ip(static_cast<word_t>(address));

      return {};
    }
  };

  template <>
  struct exec_op<opcode::cond_jump>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, address_t address, word_t flags, bool negate) noexcept
    {
      const auto cond = static_cast<bool>(reg.status.s & flags);

      if (!cond != !negate)
        reg.named.set_ip(static_cast<word_t>(address));

      return {};
    }
  };

  template <>
  struct exec_op<opcode::noop>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(Policy&, machine_registers&, machine_memory&) noexcept
    {
      return {};
    }
  };

  template <>
  struct exec_op<opcode::halt>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(Policy&, machine_registers&, machine_memory&) noexcept
    {
      return halt_result;
    }
  };

  enum class invalid_instruction_reason : int
  {
    non_zero_no_operands = 0,
    non_zero_one_operand = 1,
    non_zero_reg_two_operands = 2,
    non_zero_st_two_operands = 3,
    non_zero_unassigned_three_operands = 4,
    non_zero_unassigned_cond_operands = 5,
    non_zero_jump_addr_operands = 6,
    assignment_two_operands = 7,
  };

  [[nodiscard]] inline std::string instruction_error(const machine_registers& reg, word_t instr)
  {
    using namespace std::string_view_literals;

    std::ostringstream oss;
    output_hex(oss << "Invalid instruction 0x"sv, instr);
    output_hex(oss << " at memory location 0x"sv, static_cast<address_t>(reg.named.ip() - sizeof(word_t)));

    return std::move(oss).str();
  }

  [[nodiscard]] inline std::string nonzero_error(word_t instr, invalid_instruction_reason reason)
  {
    using namespace std::string_view_literals;

    std::ostringstream oss;
    output_hex(oss << "Invalid non-zero bits in instruction 0x"sv, instr);

    oss << " (reason: "sv << static_cast<int>(reason) << ')';

    return std::move(oss).str();
  }

  [[nodiscard]] inline std::string address_error(address_t address, std::string_view access)
  {
    using namespace std::string_view_literals;

    std::ostringstream oss;
    output_hex(oss << "Invalid "sv << access << " access to address 0x"sv, address);

    return std::move(oss).str();
  }

  struct debug_execution_policy final
  {
    static constexpr bool enabled = true;

    debugger* debugger_;

    [[nodiscard]] inline bool has_panic() const noexcept
    {
      return debugger_ && debugger_->panic();
    }

    inline void panic(std::string msg)
    {
      store_panic_or_throw(debugger_, std::move(msg));
    }

    [[nodiscard]] inline bool breakpoint(address_t /* address */) const noexcept
    {
      return false;
    }

    [[nodiscard]] inline bool data_breakpoint(address_t /* address */, word_t /* value */) const noexcept
    {
      return false;
    }
  };

  struct noop_debug_execution_policy final
  {
    static constexpr bool enabled = false;

    [[nodiscard]] inline bool has_panic() const noexcept
    {
      return false;
    }

    inline void panic(const std::string& msg)
    {
      throw_panic(msg);
    }
  };

  struct strict_execution_policy final
  {
    static constexpr bool enabled = true;

    [[nodiscard]] inline bool check_address(const machine_memory& mem, address_t address) const noexcept
    {
      return is_aligned(address) && (static_cast<memory::size_type>(address) < mem.main.size());
    }
  };

  struct noop_strict_execution_policy final
  {
    static constexpr bool enabled = false;
  };

  template <typename Profile, typename Debug, typename Strict>
  struct execution_policy final
  {
    using profile_type = Profile;

    using debug_policy = Debug;
    using strict_policy = Strict;

    [[no_unique_address]] debug_policy debug{};
    [[no_unique_address]] strict_policy strict{};

    [[nodiscard]] inline execute_result load(const machine_memory& mem, address_t address, word_t& dst)
    {
      if constexpr (strict_policy::enabled)
      {
        if (!strict.check_address(mem, address)) [[unlikely]]
          return panic(address_error(address, "read"));
      }

      dst = mem.main.load(address);

      return {};
    }

    [[nodiscard]] inline execute_result store(machine_memory& mem, address_t address, word_t value)
    {
      if constexpr (strict_policy::enabled)
      {
        if (!strict.check_address(mem, address)) [[unlikely]]
          return panic(address_error(address, "write"));
      }

      if constexpr (debug_policy::enabled)
      {
        if (debug.data_breakpoint(address, value)) [[unlikely]]
          return breakpoint_result;
      }

      mem.main.store(address, value);

      return {};
    }

    [[nodiscard]] inline execute_result check(std::pair<execute_result, optype> result, [[maybe_unused]] word_t instr)
    {
      if constexpr (strict_policy::enabled)
      {
        if (!debug.has_panic()) [[likely]]
        {
          switch (result.second)
          {
          case optype::basic:
          {
            if (instr & operand_mask) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_no_operands));
          }
          break;

          case optype::op0:
          {
            if (instr & (operand_op1_mask | operand_op2_mask)) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_one_operand));
          }
          break;

          case optype::op0_op1:
          {
            if (instr & operand_sel_mask)
            {
              if (instr & operand_as_mask) [[unlikely]]
                return panic(nonzero_error(instr, invalid_instruction_reason::assignment_two_operands));
              else if ((instr & operand_loc_mask) && (instr & operand_st_mask)) [[unlikely]]
                return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_st_two_operands));
            }
            else
            {
              if (instr & operand_op2_mask) [[unlikely]]
                return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_reg_two_operands));
            }
          }
          break;

          case optype::op0_op1_op2:
          {
            if ((instr & operand_imm_invalid_mask) == operand_imm_invalid_mask) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_unassigned_three_operands));
          }
          break;

          case optype::jump:
          {
            if ((instr & operand_addr_loc_mask) && (instr & operand_addr_mask)) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_jump_addr_operands));
          }
          break;

          case optype::cond_jump:
          {
            if ((instr & operand_addr_loc_mask) && (instr & operand_cond_addr_mask)) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_jump_addr_operands));
            if ((instr & operand_cond_invalid_mask) == operand_cond_invalid_mask) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_unassigned_cond_operands));
          }
          break;
          }
        }
      }

      return result.first;
    }

    [[nodiscard]] inline execute_result panic(std::string msg)
    {
      debug.panic(std::move(msg));

      return breakpoint_result;
    }
  };

  template <typename Profile, typename Debug, typename Strict>
  [[nodiscard]] execution_policy<Profile, Debug, Strict> make_execution_policy(Debug debug, Strict strict)
  {
    return {std::move(debug), std::move(strict)};
  }

  template <typename Policy>
  [[nodiscard]] inline word_t load_instruction(
    Policy& policy, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    const address_t ip = static_cast<address_t>(reg.named.ip());
    reg.named.set_ip(ip + sizeof(word_t));

    word_t instr = 0x0;
    result = policy.load(mem, ip, instr);

    return instr;
  }

  [[nodiscard]] inline word_t load_short_immediate(word_t instr) noexcept
  {
    return static_cast<word_t>((instr & operand_st_mask) >> operand_st_offset);
  }

  [[nodiscard]] inline address_t load_short_address(word_t instr) noexcept
  {
    return static_cast<address_t>((instr & operand_addr_mask) >> operand_addr_word_offset);
  }

  [[nodiscard]] inline address_t load_short_cond_address(word_t instr) noexcept
  {
    return static_cast<address_t>((instr & operand_cond_addr_mask) >> operand_cond_addr_word_offset);
  }

  [[nodiscard]] inline word_t& first_operand(word_t instr, machine_registers& reg) noexcept
  {
    return reg.named.r[(instr & operand_op0_mask) >> operand_op0_offset];
  }

  [[nodiscard]] inline word_t& second_reg_operand(word_t instr, machine_registers& reg) noexcept
  {
    return reg.named.r[(instr & operand_op1_mask) >> operand_op1_offset];
  }

  [[nodiscard]] inline word_t& third_reg_operand(word_t instr, machine_registers& reg) noexcept
  {
    return reg.named.r[(instr & operand_op2_mask) >> operand_op2_offset];
  }

  template <typename Policy>
  [[nodiscard]] word_t second_operand(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    return (instr & operand_sel_mask)
             ? ((instr & operand_loc_mask) ? load_instruction(policy, reg, mem, result) : load_short_immediate(instr))
             : second_reg_operand(instr, reg);
  }

  template <typename Policy>
  [[nodiscard]] std::pair<word_t, word_t> second_third_operands(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, word_t op0, execute_result& result)
  {
    if (instr & operand_sel_mask)
    {
      const auto operands =
        (instr & operand_loc_mask)
          ? std::array<word_t, 2>{{load_instruction(policy, reg, mem, result), second_reg_operand(instr, reg)}}
          : std::array<word_t, 2>{{load_short_immediate(instr), op0}};

      const auto as = static_cast<unsigned int>((instr & operand_as_mask) >> operand_as_offset);

      return {operands[as], operands[static_cast<unsigned int>(1 - as)]};
    }
    else
    {
      return {second_reg_operand(instr, reg), third_reg_operand(instr, reg)};
    }
  }

  template <typename Policy>
  [[nodiscard]] address_t jump_address_operand(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    return (instr & operand_addr_loc_mask) ? static_cast<address_t>(load_instruction(policy, reg, mem, result))
                                           : load_short_address(instr);
  }

  template <typename Policy>
  [[nodiscard]] address_t cond_jump_address_operand(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    return (instr & operand_addr_loc_mask) ? static_cast<address_t>(load_instruction(policy, reg, mem, result))
                                           : load_short_cond_address(instr);
  }

  template <optype Type>
  struct execution_traits;

  template <>
  struct execution_traits<optype::basic>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, [[maybe_unused]] word_t instr, machine_registers& reg, machine_memory& mem)
    {
      return exec_op<Code>::execute(policy, reg, mem);
    }
  };

  template <>
  struct execution_traits<optype::op0>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      word_t& op0 = first_operand(instr, reg);

      return exec_op<Code>::execute(policy, reg, mem, op0);
    }
  };

  template <>
  struct execution_traits<optype::op0_op1>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      word_t& op0 = first_operand(instr, reg);
      const word_t op1 = second_operand(policy, instr, reg, mem, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      return exec_op<Code>::execute(policy, reg, mem, op0, op1);
    }
  };

  template <>
  struct execution_traits<optype::op0_op1_op2>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      word_t& op0 = first_operand(instr, reg);
      const auto [op1, op2] = second_third_operands(policy, instr, reg, mem, op0, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      return exec_op<Code>::execute(policy, reg, mem, op0, op1, op2);
    }
  };

  template <>
  struct execution_traits<optype::jump>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      const address_t address = jump_address_operand(policy, instr, reg, mem, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      return exec_op<Code>::execute(policy, reg, mem, address);
    }
  };

  template <>
  struct execution_traits<optype::cond_jump>
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      const address_t address = cond_jump_address_operand(policy, instr, reg, mem, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      const auto flags = static_cast<word_t>((instr & operand_cond_flag_mask) >> operand_cond_flag_offset);
      const auto negate = static_cast<bool>(instr & operand_cond_neg_mask);

      return exec_op<Code>::execute(policy, reg, mem, address, flags, negate);
    }
  };

  template <opcode Code, typename Policy>
  [[nodiscard]] std::pair<execute_result, optype> execute_opcode(
    Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
  {
    using profile_type = typename Policy::profile_type;
    using traits_type = execution_traits<profile_type::template instruction_type<Code>>;

    constexpr optype opt = profile_type::template instruction_type<Code>;

    if constexpr (profile_type::template instruction_supported<Code>)
      return {traits_type::execute<Code>(policy, instr, reg, mem), opt};
    else
      return {policy.panic(instruction_error(reg, instr)), opt};
  }

  template <typename Policy>
  [[nodiscard]] execute_result execute_instruction(Policy& policy, machine_registers& reg, machine_memory& mem)
  {
    if constexpr (Policy::debug_policy::enabled)
    {
      if (policy.debug.breakpoint(static_cast<address_t>(reg.named.ip()))) [[unlikely]]
        return breakpoint_result;
    }

    std::pair result{execute_result{}, optype::basic};

    const word_t instr = load_instruction(policy, reg, mem, result.first);

    if constexpr (Policy::debug_policy::enabled)
    {
      if (result.first.breakpoint) [[unlikely]]
        return result.first;
    }

    switch (static_cast<opcode>(instr & opcode_mask))
    {
    case opcode::move:
      result = execute_opcode<opcode::move>(policy, instr, reg, mem);
      break;
    case opcode::load:
      result = execute_opcode<opcode::load>(policy, instr, reg, mem);
      break;
    case opcode::store:
      result = execute_opcode<opcode::store>(policy, instr, reg, mem);
      break;
    case opcode::add:
      result = execute_opcode<opcode::add>(policy, instr, reg, mem);
      break;
    case opcode::add_with_carry:
      result = execute_opcode<opcode::add_with_carry>(policy, instr, reg, mem);
      break;
    case opcode::jump:
      result = execute_opcode<opcode::jump>(policy, instr, reg, mem);
      break;
    case opcode::cond_jump:
      result = execute_opcode<opcode::cond_jump>(policy, instr, reg, mem);
      break;
    case opcode::noop:
      result = execute_opcode<opcode::noop>(policy, instr, reg, mem);
      break;
    case opcode::halt:
      result = execute_opcode<opcode::halt>(policy, instr, reg, mem);
      break;
    default:
      result.first = policy.panic(instruction_error(reg, instr));
      break;
    }

    if constexpr (Policy::strict_policy::enabled)
      return policy.check(result, instr);
    else
      return result.first;
  }

} // namespace yarisc::arch::detail

#endif
