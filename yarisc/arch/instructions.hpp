/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_INSTRUCTIONS_HPP
#define YARISC_ARCH_INSTRUCTIONS_HPP

#include <yarisc/arch/types.hpp>

#include <cstddef>

namespace yarisc::arch
{
  static_assert(sizeof(word_t) == 2);

  /**
   * @brief Instruction opcode mask
   */
  inline constexpr word_t opcode_mask = 0b0000000000111111;

  /**
   * @brief Instruction operand mask
   *
   * All instructions other than jumps may have zero to three operands. These instructions have the following layout:
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
   * The operand assignment flag `as` indicates which operand the constant is assigned to:
   *
   * - `as == 0`: the immediate constant is `op1`
   * - `as == 1`: the immediate constant is `op2`
   *
   * The remaining operand is defined as following:
   *
   * - `loc == 0`: the register `op0`
   * - `loc == 1`: the register named by bits [11-9] (behavior is undefined unless bit [12] is set to zero)
   *
   * Jump instructions have a different layout:
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
   * Conditional jumps store additional flags:
   *
   * @verbatim
   *
   * [15] [14] [13-9] [8-6] [5-0]
   * aloc cneg caddr  cflag opcode
   *
   * @endverbatim
   *
   * The location flag `aloc` indicates were the address is stored:
   *
   * - `aloc == 0`: short address is in `caddr` (counted in words)
   * - `aloc == 1`: the long address is in the word following this instruction word
   *
   * The jump condition is determined using `cflag & status` (behavior is undefined unless bit [8] is set to zero):
   *
   * - `cneg == 0`: the jump is performed if `cflag & status != 0x0`
   * - `cneg == 1`: the jump is performed if `cflag & status == 0x0`
   *
   * Short jump addresses are always measured in words. Long addresses loaded from the next word are in bytes as usual.
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
   * @brief Mask for the `st` flag
   */
  inline constexpr word_t operand_st_mask = 0b0001111000000000;

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
   * @brief Mask for the address `addr`
   */
  inline constexpr word_t operand_addr_mask = 0b0111111111000000;

  /**
   * @brief Mask for the `aloc` flag
   */
  inline constexpr word_t operand_addr_loc_mask = 0b1000000000000000;

  /**
   * @brief Mask for the `cflag` flags
   */
  inline constexpr word_t operand_cond_flag_mask = 0b0000000011000000;

  /**
   * @brief Mask for the `cflag` carry flag
   */
  inline constexpr word_t operand_cond_flag_carry_mask = 0b0000000001000000;

  /**
   * @brief Mask for the `cflag` zero flag
   */
  inline constexpr word_t operand_cond_flag_zero_mask = 0b0000000010000000;

  /**
   * @brief Currently unassigned `cflag`
   */
  inline constexpr word_t operand_cond_unassigned_mask = 0b0000000100000000;

  /**
   * @brief Combination of bits when all set in a conditional jump form an invalid instruction word
   */
  inline constexpr word_t operand_cond_invalid_mask = operand_cond_unassigned_mask;

  /**
   * @brief Mask for the address `caddr`
   */
  inline constexpr word_t operand_cond_addr_mask = 0b0011111000000000;

  /**
   * @brief Mask for the `cneg` flag
   */
  inline constexpr word_t operand_cond_neg_mask = 0b0100000000000000;

  /**
   * @brief Offset in bits of the address `addr`
   */
  inline constexpr std::size_t operand_addr_offset = 6;

  /**
   * @brief Shift offset used for address `addr` that takes into account that these are word addresses
   *
   * @note
   * This has to be used together with the `operand_addr_mask` to ensure that the lowest bit is zero.
   */
  inline constexpr std::size_t operand_addr_word_offset = operand_addr_offset - 1;

  /**
   * @brief Offset in bits of the `cflag` flags
   */
  inline constexpr std::size_t operand_cond_flag_offset = 6;

  /**
   * @brief Offset in bits of the address `caddr`
   */
  inline constexpr std::size_t operand_cond_addr_offset = 9;

  /**
   * @brief Shift offset used for address `caddr` that takes into account that these are word addresses
   *
   * @note
   * This has to be used together with the `operand_cond_addr_mask` to ensure that the lowest bit is zero.
   */
  inline constexpr std::size_t operand_cond_addr_word_offset = operand_cond_addr_offset - 1;

  /**
   * @brief Instruction opcodes
   */
  enum class opcode : word_t
  {
    /**
     * @brief MOV instruction
     *
     * Moves `op1` or an immediate constant into register `op0`. Updates the zero flag.
     */
    move = 0x01,

    /**
     * @brief LDR instruction
     *
     * Loads from the address `op1` or an immediate address into register `op0`.
     */
    load = 0x02,

    /**
     * @brief LDR instruction (instruction pointer relative addressing)
     *
     * @note
     * This instruction is currently not implemented.
     */
    relative_load = 0x03,

    /**
     * @brief STR instruction
     *
     * Stores the value of register `op0` to the address `op1` or an immediate address.
     */
    store = 0x04,

    /**
     * @brief STR instruction (instruction pointer relative addressing)
     *
     * @note
     * This instruction is currently not implemented.
     */
    relative_store = 0x05,

    /**
     * @brief ADD instruction
     *
     * Adds `op1` and `op2` and stores the result in register `op0`. Updates the zero and carry flags.
     */
    add = 0x10,

    /**
     * @brief ADC instruction
     *
     * Adds `op1`, `op2`, and the carry flag and stores the result in register `op0`. Updates the zero and carry flags.
     */
    add_with_carry = 0x11,

    /**
     * @brief JMP instruction
     *
     * This instruction does an unconditional jump, i.e. it loads an immediate constant into `ip`. Note that this can
     * also be achieved with a MOV instruction. However a separate instruction is more readable and we can fit larger
     * immediate addresses in the instruction word.
     */
    jump = 0x2a,

    /**
     * @brief JMP instruction (instruction pointer relative addressing)
     */
    relative_jump = 0x2b,

    /**
     * @brief JMC/JNC/JMZ/JNZ instructions
     *
     * Conditional jump instructions have a status flags bitmask that selects on which conditions the jump shall be
     * active and a negate flag that inverts the jump condition.
     */
    cond_jump = 0x2c,

    /**
     * @brief JMC/JNC/JMZ/JNZ instructions (instruction pointer relative addressing)
     *
     * @note
     * This instruction is currently not implemented.
     */
    relative_cond_jump = 0x2d,

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
     * @brief Jump instruction
     */
    jump,

    /**
     * @brief Conditional jump instruction
     */
    cond_jump,
  };

} // namespace yarisc::arch

#endif
