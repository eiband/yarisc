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

  inline constexpr auto negative_bit_offset = 8 * sizeof(word_t) - 1;
  inline constexpr auto negative_bit_mask = static_cast<word_t>(1 << negative_bit_offset);
  inline constexpr auto negative_bit_shift = negative_bit_offset - status_register::negative_pos;

  inline constexpr auto carry_bit_offset = 8 * sizeof(word_t);
  inline constexpr auto carry_bit_mask = static_cast<double_word_t>(1 << carry_bit_offset);
  inline constexpr auto carry_bit_shift = carry_bit_offset - status_register::carry_pos;

  inline constexpr auto overflow_bit_offset = 8 * sizeof(word_t) - 1;
  inline constexpr auto overflow_bit_mask = static_cast<word_t>(1 << overflow_bit_offset);
  inline constexpr auto overflow_bit_shift = overflow_bit_offset - status_register::overflow_pos;

  [[nodiscard]] word_t get_negative_status(word_t result) noexcept
  {
    return static_cast<word_t>((result & negative_bit_mask) >> negative_bit_shift);
  }

  [[nodiscard]] word_t get_zero_status(word_t result) noexcept
  {
    return (result == 0x0) ? status_register::zero_flag : 0x0;
  }

  [[nodiscard]] word_t get_carry_status(double_word_t result) noexcept
  {
    return static_cast<word_t>((result & carry_bit_mask) >> carry_bit_shift);
  }

  [[nodiscard]] word_t get_add_overflow_status(word_t result, word_t op1, word_t op2) noexcept
  {
    return static_cast<word_t>((~(op1 ^ op2) & (op1 ^ result) & overflow_bit_mask) >> overflow_bit_shift);
  }

  /*
   * Template that is specialized for each operation of the machine
   */
  template <opcode Code>
  struct exec_op;

  template <>
  struct exec_op<opcode::add>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1, word_t op2) noexcept
    {
      const double_word_t result = static_cast<double_word_t>(op1) + static_cast<double_word_t>(op2);
      const auto result_word = static_cast<word_t>(result);

      // Write back the result
      op0 = result_word;

      return {};
    }
  };

  template <>
  struct exec_op<opcode::add_with_carry>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1, word_t op2) noexcept
    {
      const double_word_t carry = (reg.status.s & status_register::carry_flag) >> status_register::carry_pos;
      const double_word_t result = static_cast<double_word_t>(op1) + static_cast<double_word_t>(op2) + carry;
      const auto result_word = static_cast<word_t>(result);

      // Write back the result
      op0 = result_word;

      return {};
    }
  };

  template <>
  struct exec_op<opcode::adds>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1, word_t op2) noexcept
    {
      const double_word_t result = static_cast<double_word_t>(op1) + static_cast<double_word_t>(op2);
      const auto result_word = static_cast<word_t>(result);

      // Write back the result
      op0 = result_word;
      // Update the status register
      reg.status.s = get_negative_status(result_word) |              // N
                     get_zero_status(result_word) |                  // Z
                     get_carry_status(result) |                      // C
                     get_add_overflow_status(result_word, op1, op2); // V

      return {};
    }
  };

  template <>
  struct exec_op<opcode::adds_with_carry>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, word_t& op0, word_t op1, word_t op2) noexcept
    {
      const double_word_t carry = (reg.status.s & status_register::carry_flag) >> status_register::carry_pos;
      const double_word_t result = static_cast<double_word_t>(op1) + static_cast<double_word_t>(op2) + carry;
      const auto result_word = static_cast<word_t>(result);

      // Write back the result
      op0 = result_word;
      // Update the status register
      reg.status.s = get_negative_status(result_word) |              // N
                     get_zero_status(result_word) |                  // Z
                     get_carry_status(result) |                      // C
                     get_add_overflow_status(result_word, op1, op2); // V

      return {};
    }
  };

  template <>
  struct exec_op<opcode::move>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers&, machine_memory&, word_t& op0, word_t op1) noexcept
    {
      op0 = op1;

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
  struct exec_op<opcode::load_indexed>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, machine_registers&, machine_memory& mem, word_t& op0, word_t op1, word_t op2)
    {
      return policy.load(mem, static_cast<address_t>(op1 + op2), op0);
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
  struct exec_op<opcode::store_indexed>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, machine_registers&, machine_memory& mem, word_t& op0, word_t op1, word_t op2)
    {
      return policy.store(mem, static_cast<address_t>(op1 + op2), op0);
    }
  };

  template <>
  struct exec_op<opcode::branch>
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
  struct exec_op<opcode::cond_branch>
  {
    template <typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy&, machine_registers& reg, machine_memory&, address_t address, word_t cond) noexcept
    {
      bool branch = false;

      switch (cond & 0x7)
      {
      //     ┌--- C == 0: `!carry()`
      //     |┌-- N != V: `less()`
      //     ||┌- Z == 1: `zero()`
      //     vvv
      case 0b001: // equal
        branch = reg.status.zero();
        break;
      case 0b010: // less signed
        branch = reg.status.less();
        break;
      case 0b011: // less or equal signed
        branch = reg.status.less() || reg.status.zero();
        break;
      case 0b100: // less unsigned
        branch = !reg.status.carry();
        break;
      case 0b101: // less or equal unsigned
        branch = !reg.status.carry() || reg.status.zero();
        break;
      default:
        break;
      }

      // The most significant bit indicates whether the condition is inverted
      if (cond & 0x8)
        branch = !branch;

      if (branch)
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
    non_zero_branch_addr_operands = 5,
    assignment_two_operands = 6,
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

          case optype::branch:
          {
            if ((instr & operand_addr_loc_mask) && (instr & operand_addr_mask)) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_branch_addr_operands));
          }
          break;

          case optype::cond_branch:
          {
            if ((instr & operand_addr_loc_mask) && (instr & operand_cond_addr_mask)) [[unlikely]]
              return panic(nonzero_error(instr, invalid_instruction_reason::non_zero_branch_addr_operands));
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
    return unpack_signed(instr, operand_st_mask, operand_st_sign_mask, operand_st_offset);
  }

  [[nodiscard]] inline address_t load_short_address(word_t instr) noexcept
  {
    return static_cast<address_t>(unpack_signed(instr, operand_addr_mask, operand_addr_sign_mask, operand_addr_offset));
  }

  [[nodiscard]] inline address_t load_short_cond_address(word_t instr) noexcept
  {
    return static_cast<address_t>(
      unpack_signed(instr, operand_cond_addr_mask, operand_cond_addr_sign_mask, operand_cond_addr_offset));
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

  template <bool MemoryAccess, typename Policy>
  [[nodiscard]] std::pair<word_t, word_t> second_third_operands(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, word_t op0, execute_result& result)
  {
    if (instr & operand_sel_mask)
    {
      const auto as = static_cast<unsigned int>((instr & operand_as_mask) >> operand_as_offset);

      if constexpr (MemoryAccess)
      {
        // Special case for loads and stores
        return (instr & operand_loc_mask)
                 ? std::pair{second_reg_operand(instr, reg), load_instruction(policy, reg, mem, result)}
                 : std::pair{reg.named.r[as + 5], load_short_immediate(instr)};
      }
      else
      {
        const auto operands =
          (instr & operand_loc_mask)
            ? std::array<word_t, 2>{{load_instruction(policy, reg, mem, result), second_reg_operand(instr, reg)}}
            : std::array<word_t, 2>{{load_short_immediate(instr), op0}};

        return {operands[as], operands[static_cast<unsigned int>(1 - as)]};
      }
    }
    else
    {
      return {second_reg_operand(instr, reg), third_reg_operand(instr, reg)};
    }
  }

  template <typename Policy>
  [[nodiscard]] address_t branch_address_operand(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    return (instr & operand_addr_loc_mask) ? static_cast<address_t>(load_instruction(policy, reg, mem, result))
                                           : load_short_address(instr);
  }

  template <typename Policy>
  [[nodiscard]] address_t cond_branch_address_operand(
    Policy& policy, word_t instr, machine_registers& reg, const machine_memory& mem, execute_result& result)
  {
    return (instr & operand_addr_loc_mask) ? static_cast<address_t>(load_instruction(policy, reg, mem, result))
                                           : load_short_cond_address(instr);
  }

  struct execution_traits_basic
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, [[maybe_unused]] word_t instr, machine_registers& reg, machine_memory& mem)
    {
      return exec_op<Code>::execute(policy, reg, mem);
    }
  };

  struct execution_traits_op0
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      word_t& op0 = first_operand(instr, reg);

      return exec_op<Code>::execute(policy, reg, mem, op0);
    }
  };

  struct execution_traits_op0_op1
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

  template <bool MemoryAccess>
  struct execution_traits_op0_op1_op2
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      word_t& op0 = first_operand(instr, reg);
      const auto [op1, op2] = second_third_operands<MemoryAccess>(policy, instr, reg, mem, op0, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      return exec_op<Code>::execute(policy, reg, mem, op0, op1, op2);
    }
  };

  struct execution_traits_branch
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      const address_t address = branch_address_operand(policy, instr, reg, mem, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      return exec_op<Code>::execute(policy, reg, mem, address);
    }
  };

  struct execution_traits_cond_branch
  {
    template <opcode Code, typename Policy>
    [[nodiscard]] static execute_result execute(
      Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
    {
      execute_result result{};

      const address_t address = cond_branch_address_operand(policy, instr, reg, mem, result);

      if constexpr (Policy::debug_policy::enabled)
      {
        if (result.breakpoint) [[unlikely]]
          return result;
      }

      const auto cond = static_cast<word_t>((instr & operand_cond_code_mask) >> operand_cond_code_offset);

      return exec_op<Code>::execute(policy, reg, mem, address, cond);
    }
  };

  template <optype Type, bool MemoryAccess = false>
  struct execution_traits;

  template <>
  struct execution_traits<optype::basic> : execution_traits_basic
  {
  };

  template <>
  struct execution_traits<optype::op0> : execution_traits_op0
  {
  };

  template <bool MemoryAccess>
  struct execution_traits<optype::op0_op1, MemoryAccess> : execution_traits_op0_op1
  {
  };

  template <bool MemoryAccess>
  struct execution_traits<optype::op0_op1_op2, MemoryAccess> : execution_traits_op0_op1_op2<MemoryAccess>
  {
  };

  template <>
  struct execution_traits<optype::branch> : execution_traits_branch
  {
  };

  template <>
  struct execution_traits<optype::cond_branch> : execution_traits_cond_branch
  {
  };

  template <opcode Code, bool MemoryAccess = false, typename Policy>
  [[nodiscard]] std::pair<execute_result, optype> execute_opcode(
    Policy& policy, word_t instr, machine_registers& reg, machine_memory& mem)
  {
    using profile_type = typename Policy::profile_type;
    using traits_type = execution_traits<profile_type::template instruction_type<Code>, MemoryAccess>;

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
      result = execute_opcode<opcode::load, true>(policy, instr, reg, mem);
      break;
    case opcode::load_indexed:
      result = execute_opcode<opcode::load_indexed, true>(policy, instr, reg, mem);
      break;
    case opcode::store:
      result = execute_opcode<opcode::store, true>(policy, instr, reg, mem);
      break;
    case opcode::store_indexed:
      result = execute_opcode<opcode::store_indexed, true>(policy, instr, reg, mem);
      break;
    case opcode::add:
      result = execute_opcode<opcode::add>(policy, instr, reg, mem);
      break;
    case opcode::add_with_carry:
      result = execute_opcode<opcode::add_with_carry>(policy, instr, reg, mem);
      break;
    case opcode::adds:
      result = execute_opcode<opcode::adds>(policy, instr, reg, mem);
      break;
    case opcode::adds_with_carry:
      result = execute_opcode<opcode::adds_with_carry>(policy, instr, reg, mem);
      break;
    case opcode::branch:
      result = execute_opcode<opcode::branch>(policy, instr, reg, mem);
      break;
    case opcode::cond_branch:
      result = execute_opcode<opcode::cond_branch>(policy, instr, reg, mem);
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
