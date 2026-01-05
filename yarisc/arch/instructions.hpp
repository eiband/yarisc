/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_INSTRUCTIONS_HPP
#define YARISC_ARCH_INSTRUCTIONS_HPP

#include <yarisc/arch/types.hpp>

#include <cassert>
#include <cstddef>
#include <type_traits>

namespace yarisc::arch
{
  static_assert(sizeof(word_t) == 2);

  namespace detail
  {
    [[nodiscard]] inline constexpr word_t sign_extend(word_t value, word_t sign_mask) noexcept
    {
      using signed_type = std::make_signed_t<word_t>;

      // This function expects the high bits to be zero
      assert((value & ~sign_mask) < sign_mask);

      const auto v = static_cast<signed_type>(value);
      const auto s = static_cast<signed_type>(sign_mask);

      return static_cast<word_t>((v ^ s) - s);
    }

    static_assert(sign_extend(0b0000000000000000, 0b0000000000001000) == 0b0000000000000000);
    static_assert(sign_extend(0b0000000000001010, 0b0000000000001000) == 0b1111111111111010);
    static_assert(sign_extend(0b0000000000010101, 0b0000000000010000) == 0b1111111111110101);
    static_assert(sign_extend(0b0000000000010101, 0b0000000000100000) == 0b0000000000010101);

    [[nodiscard]] inline constexpr word_t unpack_signed(
      word_t instr, word_t mask, word_t sign_mask, std::size_t offset) noexcept
    {
      return sign_extend(static_cast<word_t>((instr & mask) >> offset), sign_mask);
    }

  } // namespace detail

  /**
   * @brief Instruction opcode mask
   */
  inline constexpr word_t opcode_mask = 0b0000000000111111;

  /**
   * @brief Instruction operand mask
   *
   * All instructions other than branches may have zero to three operands. These instructions have the following layout:
   *
   * @verbatim
   *
   * [15] [14-9] [8-6] [5-0]
   * sel   ops    op0  opcode
   *
   * @endverbatim
   *
   * For instructions which have no operands, `sel`, `ops`, and `op0` must all be zeros. For instructions with one
   * operand, `op0` names the register and `sel` and `ops` must be all zeros. Failing to set these bits to zeros results
   * in undefined behavior.
   *
   * The most general form has three operands. Specifying `op2` for an instruction that expects only two operands
   * results in undefined behavior. `op1` and `op2` are defined as following:
   *
   * The `sel` flag selects register or immediate constant mode.
   *
   * `sel == 0`: selects register mode. The two operands `op1` and `op2` are given in `ops`:
   *
   * @verbatim
   *
   * [14-12] [11-9]
   *   op2    op1
   *
   * @endverbatim
   *
   * `sel == 1`: selects immediate constant mode. In this case `ops` is laid out as following:
   *
   * @verbatim
   *
   * [14] [13] [12-9]
   * loc   as    st
   *
   * @endverbatim
   *
   * The location flag `loc` indicates were the immediate constant is stored:
   *
   * - `loc == 0`: short immediate constants can be stored in `st`
   * - `loc == 1`: the immediate constant is in the word following this instruction word
   *
   * The operand assignment flag `as` indicates which operand the constant is assigned to (this currently wastes a bit
   * in commutative instructions):
   *
   * - `as == 0`: the immediate constant is `op1`
   * - `as == 1`: the immediate constant is `op2`
   *
   * The remaining operand is defined as following:
   *
   * - `loc == 0`: the register `r6` as `op1` and `r5` as `op2` for loads and stores, otherwise the register `op0`
   * - `loc == 1`: the register named by bits [11-9] (behavior is undefined unless bit [12] is set to zero)
   *
   * Branch instructions have a different layout:
   *
   * @verbatim
   *
   * [15] [14-6] [5-0]
   * aloc  addr  opcode
   *
   * @endverbatim
   *
   * The location flag `aloc` indicates were the address is stored:
   *
   * - `aloc == 0`: short address is in `addr` (counted in words)
   * - `aloc == 1`: the long address is in the word following this instruction word
   *
   * Conditional branch instructions store additional flags:
   *
   * @verbatim
   *
   * [15] [14-10] [9-6] [5-0]
   * aloc  caddr  ccode opcode
   *
   * @endverbatim
   *
   * The location flag `aloc` indicates were the address is stored:
   *
   * - `aloc == 0`: short address is in `caddr` (counted in words)
   * - `aloc == 1`: the long address is in the word following this instruction word
   *
   * The valid condition codes `ccode` are:
   *
   * - `0001`: equal
   * - `0010`: less signed
   * - `0011`: less or equal signed
   * - `0100`: less unsigned
   * - `0101`: less or equal unsigned
   * - `1001`: not equal
   * - `1010`: greater or equal signed
   * - `1011`: greater signed
   * - `1100`: greater or equal unsigned
   * - `1101`: greater unsigned
   *
   * Short branch addresses are always measured in words. Long addresses loaded from the next word are in bytes as
   * usual.
   *
   * Short immediate constants and short addresses are always sign-extended to keep the decoding simple.
   */
  inline constexpr word_t operand_mask = 0b1111111111000000;

  /**
   * @brief Mask for the `op0` register
   */
  inline constexpr word_t operand_op0_mask = 0b0000000111000000;

  /**
   * @brief Mask for the `op1` register
   */
  inline constexpr word_t operand_op1_mask = 0b0000111000000000;

  /**
   * @brief Mask for the `op2` register
   */
  inline constexpr word_t operand_op2_mask = 0b0111000000000000;

  /**
   * @brief Mask for the `sel` flag
   */
  inline constexpr word_t operand_sel_mask = 0b1000000000000000;

  /**
   * @brief Mask for the `loc` flag
   */
  inline constexpr word_t operand_loc_mask = 0b0100000000000000;

  /**
   * @brief Mask for the `as` flag
   */
  inline constexpr word_t operand_as_mask = 0b0010000000000000;

  /**
   * @brief Mask for the `st` field
   */
  inline constexpr word_t operand_st_mask = 0b0001111000000000;

  /**
   * @brief Sign mask for the `st` field after the shift
   */
  inline constexpr word_t operand_st_sign_mask = 0b0000000000001000;

  /**
   * @brief Bitmask for an immediate constant stored in the next word
   */
  inline constexpr word_t operand_imm_mask = operand_loc_mask | operand_sel_mask;

  /**
   * @brief Bit that currently has no meaning when the immediate constant is in the next word
   */
  inline constexpr word_t operand_imm_unassigned_mask = 0b0001000000000000;

  /**
   * @brief Combination of bits when all set as operands form an invalid instruction word
   */
  inline constexpr word_t operand_imm_invalid_mask = operand_imm_mask | operand_imm_unassigned_mask;

  /**
   * @brief Offset in bits of the `op0` register
   */
  inline constexpr std::size_t operand_op0_offset = 6;

  /**
   * @brief Offset in bits of the `op1` register
   */
  inline constexpr std::size_t operand_op1_offset = 9;

  /**
   * @brief Offset in bits of the `op2` register
   */
  inline constexpr std::size_t operand_op2_offset = 12;

  /**
   * @brief Offset in bits of the `st` field
   */
  inline constexpr std::size_t operand_st_offset = 9;

  /**
   * @brief Offset in bits of the `as` flag
   */
  inline constexpr std::size_t operand_as_offset = 13;

  /**
   * @brief Mask for the `aloc` flag
   */
  inline constexpr word_t operand_addr_loc_mask = 0b1000000000000000;

  /**
   * @brief Mask for the address `addr`
   */
  inline constexpr word_t operand_addr_mask = 0b0111111111000000;

  /**
   * @brief Sign mask for the address `addr` after the shift
   */
  inline constexpr word_t operand_addr_sign_mask = 0b0000001000000000;

  /**
   * @brief Mask for the address `caddr`
   */
  inline constexpr word_t operand_cond_addr_mask = 0b0111110000000000;

  /**
   * @brief Sign mask for the address `caddr` after the shift
   */
  inline constexpr word_t operand_cond_addr_sign_mask = 0b0000000000100000;

  /**
   * @brief Mask for the condition code `ccode`
   */
  inline constexpr word_t operand_cond_code_mask = 0b0000001111000000;

  /**
   * @brief Shift offset used for address `addr` that takes into account that these are word addresses
   *
   * @note
   * This has to be used together with the `operand_addr_mask` to ensure that the lowest bit is zero.
   */
  inline constexpr std::size_t operand_addr_offset = 5;

  /**
   * @brief Shift offset used for address `caddr` that takes into account that these are word addresses
   *
   * @note
   * This has to be used together with the `operand_cond_addr_mask` to ensure that the lowest bit is zero.
   */
  inline constexpr std::size_t operand_cond_addr_offset = 9;

  /**
   * @brief Offset in bits of the condition code `ccode`
   */
  inline constexpr std::size_t operand_cond_code_offset = 6;

  /**
   * @brief Instruction opcodes
   */
  enum class opcode : word_t
  {
    /**
     * @brief MOV instruction
     *
     * Moves `op1` or an immediate constant into register `op0`.
     */
    move = 0x01,

    /**
     * @brief LDR instruction
     *
     * Loads from the address `op1 + op2` into register `op0`. See special handling of short immediate constants which
     * allow to select the stack pointer or frame pointer.
     */
    load = 0x02,

    /**
     * @brief LDX instruction
     *
     * Loads from the address `op1 + op2` into register `op0`. See special handling of short immediate constants which
     * allow to select the stack pointer or frame pointer.
     */
    load_indexed = 0x03,

    /**
     * @brief STR instruction
     *
     * Stores the value of register `op0` to the address `op1 + op2`. See special handling of short immediate constants
     * which allow to select the stack pointer or frame pointer.
     */
    store = 0x04,

    /**
     * @brief STX instruction
     *
     * Stores the value of register `op0` to the address `op1 + op2`. See special handling of short immediate constants
     * which allow to select the stack pointer or frame pointer.
     */
    store_indexed = 0x05,

    /**
     * @brief ADD instruction
     *
     * Adds `op1` and `op2` and stores the result in register `op0`.
     */
    add = 0x10,

    /**
     * @brief ADC instruction
     *
     * Adds `op1`, `op2`, and the carry flag and stores the result in register `op0`.
     */
    add_with_carry = 0x11,

    /**
     * @brief ADD instruction
     *
     * Adds `op1` and `op2` and stores the result in register `op0`. Updates the zero and carry flags.
     */
    adds = 0x12,

    /**
     * @brief ADC instruction
     *
     * Adds `op1`, `op2`, and the carry flag and stores the result in register `op0`. Updates the zero and carry flags.
     */
    adds_with_carry = 0x13,

    /**
     * @brief BRA instruction
     *
     * This instruction does an unconditional branch, i.e. it loads an immediate constant into `ip`. Note that this can
     * also be achieved with a MOV instruction. However a separate instruction is more readable and we can fit larger
     * immediate addresses in the instruction word.
     */
    branch = 0x2a,

    /**
     * @brief BEQ/BLT/BLE/BLO/BLS/BNE/BGE/BGT/BHS/BHI instructions
     *
     * Conditional branch instructions have a status flags bitmask that selects on which conditions the branch shall be
     * active and a invert flag that inverts the branch condition.
     */
    cond_branch = 0x2c,

    /**
     * @brief NOP instruction
     */
    noop = 0x3e,

    /**
     * @brief HLT instruction
     *
     * Halts the processor.
     */
    halt = 0x3f,
  };

  /**
   * @brief Type of instruction
   */
  enum class optype
  {
    /**
     * @brief Instruction without operands
     */
    basic,

    /**
     * @brief Instruction with one operand
     */
    op0,

    /**
     * @brief Instruction with two operands
     */
    op0_op1,

    /**
     * @brief Instruction with three operands
     */
    op0_op1_op2,

    /**
     * @brief Branch instruction
     */
    branch,

    /**
     * @brief Conditional branch instruction
     */
    cond_branch,
  };

} // namespace yarisc::arch

#endif
