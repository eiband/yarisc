/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_ASSEMBLY_HPP
#define YARISC_ARCH_ASSEMBLY_HPP

#include <yarisc/arch/export.h>
#include <yarisc/arch/feature_level.hpp>
#include <yarisc/arch/instructions.hpp>
#include <yarisc/arch/machine_profile.hpp>
#include <yarisc/arch/types.hpp>

#include <cassert>
#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>

namespace yarisc::arch
{
  namespace detail
  {
    [[noreturn]] YARISC_ARCH_EXPORT void throw_invalid_immediate(word_t imm, word_t mask);

  } // namespace detail

  namespace assembly
  {
    using opcode = opcode;

    /**
     * @brief Register address
     */
    enum class regaddr : std::uint8_t
    {
      r0 = 0x0,
      r1 = 0x1,
      r2 = 0x2,
      r3 = 0x3,
      r4 = 0x4,
      r5 = 0x5,
      sp = 0x6,
      ip = 0x7,
    };

    inline constexpr regaddr r0 = regaddr::r0;
    inline constexpr regaddr r1 = regaddr::r1;
    inline constexpr regaddr r2 = regaddr::r2;
    inline constexpr regaddr r3 = regaddr::r3;
    inline constexpr regaddr r4 = regaddr::r4;
    inline constexpr regaddr r5 = regaddr::r5;
    inline constexpr regaddr sp = regaddr::sp;
    inline constexpr regaddr ip = regaddr::ip;

    /**
     * @brief Jump condition of conditional jumps
     */
    enum class jump_condition : word_t
    {
      jc = operand_cond_flag_carry_mask,
      jz = operand_cond_flag_zero_mask,
      jnc = operand_cond_flag_carry_mask | operand_cond_neg_mask,
      jnz = operand_cond_flag_zero_mask | operand_cond_neg_mask,
    };

    inline constexpr jump_condition jc = jump_condition::jc;
    inline constexpr jump_condition jz = jump_condition::jz;
    inline constexpr jump_condition jnc = jump_condition::jnc;
    inline constexpr jump_condition jnz = jump_condition::jnz;

    /**
     * @brief Placeholder type for the register passed as first operand
     */
    class accumulator_t final
    {
    public:
      /**
       * @brief Constructor
       */
      explicit accumulator_t() noexcept = default;
    };

    /**
     * @brief Placeholder constant for the register passed as first operand
     *
     * This placeholder is used together with short immediate constants in binary operations. The remaining operand
     * implicitly is the same as the first operand effectively writing the result back into the first operand register.
     *
     * So the accumulator is not a register on its own, it is an alias for the first operand.
     */
    inline constexpr accumulator_t accumulator{};

    /**
     * @brief Placeholder type to signal that the immediate constant is stored in the next word
     */
    class immediate_t final
    {
    public:
      /**
       * @brief Constructor
       */
      explicit immediate_t() noexcept = default;
    };

    /**
     * @brief Placeholder constant to signal that the immediate constant is stored in the next word
     */
    inline constexpr immediate_t immediate{};

    /**
     * @brief Value checked immediate constant
     */
    template <word_t Mask, word_t SignMask>
    class checked_immediate final
    {
    public:
      static_assert(SignMask > Mask);
      static_assert((Mask & SignMask) == 0);

      /**
       * @brief Constructor
       *
       * Creates an immediate with value 0.
       */
      explicit checked_immediate() noexcept = default;

      /**
       * @brief Constructor
       *
       * Creates an immediate with value `imm`. Throws an exception if the value is not a signed value with the sign at
       * `SignMask` and only non-sign bits of `Mask` set.
       *
       * @param imm value of the immediate constant
       */
      explicit checked_immediate(word_t imm)
        : value_{imm}
      {
        if (!check(value_))
          detail::throw_invalid_immediate(value_, Mask | SignMask);
      }

      /**
       * @brief Returns the immediate constant value
       */
      [[nodiscard]] word_t get() const noexcept
      {
        return value_;
      }

      /**
       * @brief Creates an immediate constant without runtime checks
       *
       * Behavior is undefined unless `imm` is a signed value with the sign at `SignMask` and only non-sign bits of
       * `Mask` set.
       *
       * @param imm value of the immediate constant
       * @return immediate constant with given value
       */
      [[nodiscard]] static checked_immediate unchecked(word_t imm) noexcept
      {
        assert(check(imm));

        checked_immediate result;
        result.value_ = imm;

        return result;
      }

    private:
      word_t value_{0x0};

      [[nodiscard]] static bool check(word_t value) noexcept
      {
        return (detail::sign_extend(value & (Mask | SignMask), SignMask) == value);
      }
    };

    static_assert((sizeof(word_t) == 2), "Invalid address masks");

    /**
     * @brief Short immediate constant that can be stored in the instruction word
     */
    using short_immediate = checked_immediate<0x7, 0x8>;

    /**
     * @brief Short immediate jump address that can be stored in the instruction word
     */
    using short_jump_address = checked_immediate<0x01fe, 0x0200>;

    /**
     * @brief Short immediate conditional jump address that can be stored in the instruction word
     */
    using short_cond_jump_address = checked_immediate<0x1e, 0x20>;

  } // namespace assembly

  namespace detail
  {
    template <typename T, typename U>
    concept assemble_value = std::same_as<std::remove_cvref_t<T>, U>;

    template <typename T>
    concept indirect_immediate = assemble_value<T, assembly::immediate_t>;

  } // namespace detail

  /**
   * @brief Either a short immediate or an immediate placeholder
   */
  template <typename T>
  concept immediate_operand = detail::indirect_immediate<T> || detail::assemble_value<T, assembly::short_immediate>;

  /**
   * @brief Operand type that is either a register address, a short immediate, or an immediate placeholder
   */
  template <typename T>
  concept unary_operand = immediate_operand<T> || detail::assemble_value<T, assembly::regaddr>;

  /**
   * @brief Operand type that in addition can be an accumulator placeholder
   */
  template <typename T>
  concept binary_operand = unary_operand<T> || detail::assemble_value<T, assembly::accumulator_t>;

  /**
   * @brief Short immediate jump address operand
   */
  template <typename T>
  concept jump_operand = detail::indirect_immediate<T> || detail::assemble_value<T, assembly::short_jump_address>;

  /**
   * @brief Short immediate conditional jump address operand
   */
  template <typename T>
  concept cond_jump_operand =
    detail::indirect_immediate<T> || detail::assemble_value<T, assembly::short_cond_jump_address>;

  namespace detail
  {
    using assembly::accumulator_t;
    using assembly::immediate_t;
    using assembly::jump_condition;
    using assembly::regaddr;
    using assembly::short_cond_jump_address;
    using assembly::short_immediate;
    using assembly::short_jump_address;

    [[nodiscard]] inline word_t make_op0(regaddr addr) noexcept
    {
      return (static_cast<word_t>(static_cast<std::uint8_t>(addr)) << operand_op0_offset) & operand_op0_mask;
    }

    [[nodiscard]] inline word_t make_op1(regaddr addr) noexcept
    {
      return (static_cast<word_t>(static_cast<std::uint8_t>(addr)) << operand_op1_offset) & operand_op1_mask;
    }

    [[nodiscard]] inline word_t make_op2(regaddr addr) noexcept
    {
      return (static_cast<word_t>(static_cast<std::uint8_t>(addr)) << operand_op2_offset) & operand_op2_mask;
    }

    [[nodiscard]] inline word_t make_immediate(short_immediate imm) noexcept
    {
      return (imm.get() << operand_st_offset) & operand_st_mask;
    }

    [[nodiscard]] inline word_t make_immediate(short_jump_address address) noexcept
    {
      return (address.get() << operand_addr_offset) & operand_addr_mask;
    }

    [[nodiscard]] inline word_t make_immediate(short_cond_jump_address address) noexcept
    {
      return (address.get() << operand_cond_addr_offset) & operand_cond_addr_mask;
    }

    [[nodiscard]] inline word_t make_condition(jump_condition cond) noexcept
    {
      return static_cast<word_t>(cond) & (operand_cond_neg_mask | operand_cond_flag_mask);
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0) noexcept
    {
      return make_op0(op0);
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, regaddr op1) noexcept
    {
      return make_op0(op0) | make_op1(op1);
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, immediate_t) noexcept
    {
      return make_op0(op0) | operand_imm_mask;
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, short_immediate op1) noexcept
    {
      return make_op0(op0) | make_immediate(op1) | operand_sel_mask;
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, regaddr op1, regaddr op2) noexcept
    {
      return make_op0(op0) | make_op1(op1) | make_op2(op2);
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, immediate_t, regaddr op2) noexcept
    {
      return make_op0(op0) | make_op1(op2) | operand_imm_mask;
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, regaddr op1, immediate_t) noexcept
    {
      return make_op0(op0) | make_op1(op1) | operand_as_mask | operand_imm_mask;
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, short_immediate op1, accumulator_t) noexcept
    {
      return make_op0(op0) | make_immediate(op1) | operand_sel_mask;
    }

    [[nodiscard]] inline word_t make_operands(regaddr op0, accumulator_t, short_immediate op2) noexcept
    {
      return make_op0(op0) | make_immediate(op2) | operand_as_mask | operand_sel_mask;
    }

    [[nodiscard]] inline word_t make_jump_operands(immediate_t) noexcept
    {
      return operand_addr_loc_mask;
    }

    [[nodiscard]] inline word_t make_jump_operands(short_jump_address address) noexcept
    {
      return make_immediate(address);
    }

    [[nodiscard]] inline word_t make_jump_operands(jump_condition cond, immediate_t) noexcept
    {
      return make_condition(cond) | operand_addr_loc_mask;
    }

    [[nodiscard]] inline word_t make_jump_operands(jump_condition cond, short_cond_jump_address address) noexcept
    {
      return make_condition(cond) | make_immediate(address);
    }

  } // namespace detail

  /**
   * @brief Assembles a basic instruction without operands
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::basic, machine_profile<Level>>
  [[nodiscard]] word_t assemble() noexcept
  {
    return static_cast<word_t>(Code);
  }

  /**
   * @brief Assembles an instruction with one operands
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::op0, machine_profile<Level>>
  [[nodiscard]] auto assemble(assembly::regaddr op0) noexcept -> decltype(detail::make_operands(op0))
  {
    return static_cast<word_t>(Code) | detail::make_operands(op0);
  }

  /**
   * @brief Assembles an instruction with two operands
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::op0_op1, machine_profile<Level>>
  [[nodiscard]] auto assemble(assembly::regaddr op0, unary_operand auto op1) noexcept
    -> decltype(detail::make_operands(op0, op1))
  {
    return static_cast<word_t>(Code) | detail::make_operands(op0, op1);
  }

  /**
   * @brief Assembles an instruction with three operands
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::op0_op1_op2, machine_profile<Level>>
  [[nodiscard]] auto assemble(assembly::regaddr op0, binary_operand auto op1, binary_operand auto op2) noexcept
    -> decltype(detail::make_operands(op0, op1, op2))
  {
    return static_cast<word_t>(Code) | detail::make_operands(op0, op1, op2);
  }

  /**
   * @brief Assembles a jump instruction
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::jump, machine_profile<Level>>
  [[nodiscard]] auto assemble(jump_operand auto address) noexcept -> decltype(detail::make_jump_operands(address))
  {
    return static_cast<word_t>(Code) | detail::make_jump_operands(address);
  }

  /**
   * @brief Assembles a jump instruction
   */
  template <opcode Code, feature_level Level = feature_level_latest>
    requires opcode_of_type<Code, optype::cond_jump, machine_profile<Level>>
  [[nodiscard]] auto assemble(assembly::jump_condition cond, cond_jump_operand auto address) noexcept
    -> decltype(detail::make_jump_operands(cond, address))
  {
    return static_cast<word_t>(Code) | detail::make_jump_operands(cond, address);
  }

  /**
   * @brief Result of disassembling an instruction
   */
  struct disassembly final
  {
    /**
     * @brief Number of words consumed (0 if there was an error)
     */
    int words{0};

    /**
     * @brief Textual representation of the instruction or the error
     */
    std::string text{};
  };

  /**
   * @brief Disassembles an instruction
   *
   * @param instr instruction word to disassemble
   * @param arg word following the instruction word
   * @param level feature level for which to emit the code
   * @return textual representation of the instruction
   */
  [[nodiscard]] YARISC_ARCH_EXPORT disassembly
    disassemble(word_t instr, word_t arg, feature_level level = feature_level_latest);

} // namespace yarisc::arch

#endif
