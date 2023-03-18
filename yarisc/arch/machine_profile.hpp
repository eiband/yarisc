/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_MACHINE_PROFILE_HPP
#define YARISC_ARCH_MACHINE_PROFILE_HPP

#include <yarisc/arch/feature_level.hpp>
#include <yarisc/arch/instructions.hpp>
#include <yarisc/arch/types.hpp>

#include <array>
#include <cstddef>
#include <string_view>

namespace yarisc::arch
{
  namespace detail
  {
    struct instruction_descriptor final
    {
      std::string_view mnemonic{};
      feature_level level{feature_level::none};
      optype type{optype::basic};
    };

    inline constexpr std::size_t num_opcodes = static_cast<std::size_t>(opcode_mask) + 1;

    static_assert(num_opcodes == 64);

    inline constexpr std::array<instruction_descriptor, num_opcodes> instruction_table{{
      /* 0x00 */ {},
      /* 0x01 */ {"MOV", feature_level::min, optype::op0_op1},
      /* 0x02 */ {"LDR", feature_level::min, optype::op0_op1},
      /* 0x03 */ {},
      /* 0x04 */ {"STR", feature_level::min, optype::op0_op1},
      /* 0x05 */ {},
      /* 0x06 */ {},
      /* 0x07 */ {},
      /* 0x08 */ {},
      /* 0x09 */ {},
      /* 0x0a */ {},
      /* 0x0b */ {},
      /* 0x0c */ {},
      /* 0x0d */ {},
      /* 0x0e */ {},
      /* 0x0f */ {},
      /* 0x10 */ {"ADD", feature_level::min, optype::op0_op1_op2},
      /* 0x11 */ {"ADC", feature_level::min, optype::op0_op1_op2},
      /* 0x12 */ {},
      /* 0x13 */ {},
      /* 0x14 */ {},
      /* 0x15 */ {},
      /* 0x16 */ {},
      /* 0x17 */ {},
      /* 0x18 */ {},
      /* 0x19 */ {},
      /* 0x1a */ {},
      /* 0x1b */ {},
      /* 0x1c */ {},
      /* 0x1d */ {},
      /* 0x1e */ {},
      /* 0x1f */ {},
      /* 0x20 */ {},
      /* 0x21 */ {},
      /* 0x22 */ {},
      /* 0x23 */ {},
      /* 0x24 */ {},
      /* 0x25 */ {},
      /* 0x26 */ {},
      /* 0x27 */ {},
      /* 0x28 */ {},
      /* 0x29 */ {},
      /* 0x2a */ {"JMP", feature_level::v1, optype::jump},
      /* 0x2b */ {},
      /* 0x2c */ {"J", feature_level::min, optype::cond_jump},
      /* 0x2d */ {},
      /* 0x2e */ {},
      /* 0x2f */ {},
      /* 0x30 */ {},
      /* 0x31 */ {},
      /* 0x32 */ {},
      /* 0x33 */ {},
      /* 0x34 */ {},
      /* 0x35 */ {},
      /* 0x36 */ {},
      /* 0x37 */ {},
      /* 0x38 */ {},
      /* 0x39 */ {},
      /* 0x3a */ {},
      /* 0x3b */ {},
      /* 0x3c */ {},
      /* 0x3d */ {},
      /* 0x3e */ {"NOP", feature_level::v1, optype::basic},
      /* 0x3f */ {"HLT", feature_level::min, optype::basic},
    }};

    template <opcode Code>
    inline constexpr instruction_descriptor instruction_descriptor_v =
      instruction_table[static_cast<word_t>(Code) & opcode_mask];

    template <opcode Code>
    static constexpr optype instruction_type_v = instruction_descriptor_v<Code>.type;
    template <opcode Code>
    static constexpr feature_level instruction_level_v = instruction_descriptor_v<Code>.level;
    template <opcode Code>
    static constexpr std::string_view instruction_mnemonic_v = instruction_descriptor_v<Code>.mnemonic;

    template <opcode Code, feature_level Level>
    static constexpr bool instruction_supported_v =
      (static_cast<feature_level_t>(instruction_level_v<Code>) <= static_cast<feature_level_t>(Level));

  } // namespace detail

  /**
   * @brief The profile provides all information on different variations of the machine
   */
  template <feature_level Level>
  struct machine_profile final
  {
    /**
     * @brief Feature level of the profile
     */
    static constexpr feature_level level = Level;

    /**
     * @brief Type of an instruction, i.e. the instruction format
     */
    template <opcode Code>
    static constexpr optype instruction_type = detail::instruction_type_v<Code>;

    /**
     * @brief Mnemonic of the instruction, e.g. "MOV"
     *
     * @note
     * For conditional jumps, the mnemonic is just the prefix for the jump condition mnemonic.
     */
    template <opcode Code>
    static constexpr std::string_view instruction_mnemonic = detail::instruction_mnemonic_v<Code>;

    /**
     * @brief Boolean whether an instruction is supported
     */
    template <opcode Code>
    static constexpr bool instruction_supported = detail::instruction_supported_v<Code, Level>;
  };

  /**
   * @brief Instruction opcode of certain type in the given profile
   */
  template <opcode Code, optype Type, typename Profile>
  concept opcode_of_type = (Profile::template instruction_supported<Code>) &&
                           (Profile::template instruction_type<Code> == Type);

} // namespace yarisc::arch

#endif
