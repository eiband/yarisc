/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_REGISTERS_HPP
#define YARISC_ARCH_REGISTERS_HPP

#include <yarisc/arch/types.hpp>

#include <array>
#include <string_view>

namespace yarisc::arch
{
  /**
   * @brief Named registers of the machine
   *
   * The first three registers `r0`, `r1`, and `r2` are volatile. `r0` and `r1` are used for arguments and the return
   * value when a subroutine is called. `r5` may contain the frame pointer. Other than that, `r0`, `r1`, and `r2` can
   * also be used as scratch registers.
   *
   * The registers `r3`, `r4`, and `r5` are general purpose registers. They are also non-volatile in the calling
   * convention, i.e. the callee needs to preserve their value.
   *
   * The register `r6` is the stack pointer. The stack stack grows downwards.
   *
   * The register `r7` is the instruction pointer. It points to the next instruction to be executed.
   */
  struct registers final
  {
    std::array<word_t, num_registers> r{};

    static_assert(num_registers == 8);

    [[nodiscard]] word_t r0() const noexcept
    {
      return r[0]; // r0: general purpose
    }

    [[nodiscard]] word_t r1() const noexcept
    {
      return r[1]; // r1: general purpose
    }

    [[nodiscard]] word_t r2() const noexcept
    {
      return r[2]; // r2: general purpose
    }

    [[nodiscard]] word_t r3() const noexcept
    {
      return r[3]; // r3: general purpose
    }

    [[nodiscard]] word_t r4() const noexcept
    {
      return r[4]; // r4: general purpose
    }

    [[nodiscard]] word_t r5() const noexcept
    {
      return r[5]; // r5: frame pointer
    }

    [[nodiscard]] word_t sp() const noexcept
    {
      return r[6]; // r6: stack pointer
    }

    [[nodiscard]] word_t ip() const noexcept
    {
      return r[7]; // r7: instruction pointer
    }

    void set_r0(word_t w) noexcept
    {
      r[0] = w;
    }

    void set_r1(word_t w) noexcept
    {
      r[1] = w;
    }

    void set_r2(word_t w) noexcept
    {
      r[2] = w;
    }

    void set_r3(word_t w) noexcept
    {
      r[3] = w;
    }

    void set_r4(word_t w) noexcept
    {
      r[4] = w;
    }

    void set_r5(word_t w) noexcept
    {
      r[5] = w;
    }

    void set_sp(word_t w) noexcept
    {
      r[6] = w;
    }

    void set_ip(word_t w) noexcept
    {
      r[7] = w;
    }

    [[nodiscard]] bool operator==(const registers& that) const noexcept = default;
  };

  /**
   * @brief The status register holds information about the last arithmetic operation
   */
  struct status_register final
  {
    static constexpr unsigned int negative_pos = 0;
    static constexpr unsigned int zero_pos = 1;
    static constexpr unsigned int carry_pos = 2;
    static constexpr unsigned int overflow_pos = 3;

    static constexpr word_t negative_flag = 0x1 << negative_pos;
    static constexpr word_t zero_flag = 0x1 << zero_pos;
    static constexpr word_t carry_flag = 0x1 << carry_pos;
    static constexpr word_t overflow_flag = 0x1 << overflow_pos;

    static constexpr word_t mask = negative_flag | zero_flag | carry_flag | overflow_flag;

    word_t s{0};

    [[nodiscard]] bool negative() const noexcept
    {
      return ((s & negative_flag) != 0);
    }

    [[nodiscard]] bool zero() const noexcept
    {
      return ((s & zero_flag) != 0);
    }

    [[nodiscard]] bool carry() const noexcept
    {
      return ((s & carry_flag) != 0);
    }

    [[nodiscard]] bool overflow() const noexcept
    {
      return ((s & overflow_flag) != 0);
    }

    [[nodiscard]] bool less() const noexcept
    {
      static_assert(negative_pos == 0);

      // Shift the overflow flag down to the least significant bit and compare it with the negative flag
      return (((s & overflow_flag) >> overflow_pos) != (s & negative_flag));
    }

    void set_negative() noexcept
    {
      s |= negative_flag;
    }

    void set_negative(bool c) noexcept
    {
      s = c ? (s | negative_flag) : (s & ~negative_flag);
    }

    void unset_negative() noexcept
    {
      s &= ~negative_flag;
    }

    void set_zero() noexcept
    {
      s |= zero_flag;
    }

    void set_zero(bool c) noexcept
    {
      s = c ? (s | zero_flag) : (s & ~zero_flag);
    }

    void unset_zero() noexcept
    {
      s &= ~zero_flag;
    }

    void set_carry() noexcept
    {
      s |= carry_flag;
    }

    void set_carry(bool c) noexcept
    {
      s = c ? (s | carry_flag) : (s & ~carry_flag);
    }

    void unset_carry() noexcept
    {
      s &= ~carry_flag;
    }

    void set_overflow() noexcept
    {
      s |= overflow_flag;
    }

    void set_overflow(bool c) noexcept
    {
      s = c ? (s | overflow_flag) : (s & ~overflow_flag);
    }

    void unset_overflow() noexcept
    {
      s &= ~overflow_flag;
    }

    [[nodiscard]] bool operator==(const status_register& that) const noexcept = default;
  };

} // namespace yarisc::arch

#endif
