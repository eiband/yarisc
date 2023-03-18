/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_TYPES_HPP
#define YARISC_ARCH_TYPES_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace yarisc::arch
{
  /**
   * @brief Type of a single word of the machine
   *
   * This is also the word size of registers and operand size of the ALU.
   */
  using word_t = std::uint16_t;

  /**
   * @brief Array of bytes of one word of memory
   *
   * @note
   * The machine is little-endian, so the least-significant byte comes first in this array.
   */
  using word_as_bytes_t = std::array<std::byte, sizeof(word_t)>;

  /**
   * @brief Type twice as big as a machine word
   *
   * @note
   * This type does not exist inside the machine.
   */
  using double_word_t = std::uint32_t;

  /**
   * @brief Type of a memory address of the machine
   *
   * The address points to bytes. However, this version of the machine is not capable of unaligned memory accesses. Any
   * unaligned memory loads or stores result in a panic.
   */
  using address_t = std::uint16_t;

  /**
   * @brief Number of named registers
   */
  inline constexpr std::size_t num_registers = 8;

} // namespace yarisc::arch

#endif
