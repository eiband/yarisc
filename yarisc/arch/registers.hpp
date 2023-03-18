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
   * The first three registers `r0`, `r1`, and `r2` are general purpose registers. They are also non-volatile in the
   * standard calling convention, i.e. the subroutine needs to preserve their value.
   *
   * The registers `r3`, `r4`, and `r5` are volatile. The latter `r4` and `r5` are used when a subroutine is called. In
   * the standard calling convention, `r4` points to the result storage. `r5` contains the return address to which the
   * subroutine needs to return. Other than that, `r4` and `r5` can also be used as scratch registers just liek `r3`.
   *
   * The register `r6` is the stack pointer. The stack stack grows downwards. `r6` can be used as scratch register if
   * the whole program doesn't use a stack.
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
      return r[3]; // r3: scratch purpose
    }

    [[nodiscard]] word_t r4() const noexcept
    {
      return r[4]; // r4: result pointer
    }

    [[nodiscard]] word_t r5() const noexcept
    {
      return r[5]; // r5: return pointer
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
    static constexpr unsigned int carry_pos = 0;
    static constexpr unsigned int zero_pos = 1;

    static constexpr word_t carry_flag = 0x1 << carry_pos;
    static constexpr word_t zero_flag = 0x1 << zero_pos;

    static constexpr word_t mask = carry_flag | zero_flag;

    word_t s{0};

    [[nodiscard]] bool carry() const noexcept
    {
      return ((s & carry_flag) != 0);
    }

    [[nodiscard]] bool zero() const noexcept
    {
      return ((s & zero_flag) != 0);
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

    [[nodiscard]] bool operator==(const status_register& that) const noexcept = default;
  };

} // namespace yarisc::arch

#endif
