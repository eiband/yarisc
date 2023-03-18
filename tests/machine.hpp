/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_TESTS_MACHINE_HPP
#define YARISC_TESTS_MACHINE_HPP

#include <yarisc/arch/debugger.hpp>
#include <yarisc/arch/machine_model.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/arch/types.hpp>

#include <memory>
#include <string>

namespace yarisc::test
{
  inline constexpr arch::word_t status_c = arch::status_register::carry_flag;
  inline constexpr arch::word_t status_z = arch::status_register::zero_flag;
  inline constexpr arch::word_t status_zc = arch::status_register::zero_flag | arch::status_register::carry_flag;

  /**
   * @brief Test machine
   */
  class machine final
  {
  public:
    using size_type = arch::memory::size_type;

    machine();

    explicit machine(arch::word_t word);
    machine(arch::word_t word0, arch::word_t word1);

    [[nodiscard]] arch::word_t load(size_type off) const;

    bool store(size_type off, arch::word_t word);

    static_assert(arch::num_registers == 8);

    void set_status(arch::word_t word) noexcept
    {
      registers_.status.s = word;
    }

    void clear_status() noexcept
    {
      registers_.status.s = 0x0;
    }

    void set_r0(arch::word_t word) noexcept
    {
      registers_.named.set_r0(word);
    }

    void set_r1(arch::word_t word) noexcept
    {
      registers_.named.set_r1(word);
    }

    void set_r2(arch::word_t word) noexcept
    {
      registers_.named.set_r2(word);
    }

    void set_r3(arch::word_t word) noexcept
    {
      registers_.named.set_r3(word);
    }

    void set_r4(arch::word_t word) noexcept
    {
      registers_.named.set_r4(word);
    }

    void set_r5(arch::word_t word) noexcept
    {
      registers_.named.set_r5(word);
    }

    void set_sp(arch::address_t address) noexcept
    {
      registers_.named.set_sp(static_cast<arch::word_t>(address));
    }

    void set_ip(arch::address_t address) noexcept
    {
      registers_.named.set_ip(static_cast<arch::word_t>(address));
    }

    void advance_ip(arch::word_t num_words = 1) noexcept
    {
      set_ip(static_cast<arch::address_t>(registers_.named.ip() + num_words * sizeof(arch::word_t)));
    }

    bool execute_instruction(bool throw_on_breakpoint = true);

    [[nodiscard]] std::string disassemble_instruction(int expected_words = 1) const;

    [[nodiscard]] bool operator==(const machine& that) const noexcept = default;

  private:
    static constexpr arch::word_t initial_sp = 0x5f;
    static constexpr arch::word_t initial_ip = 0x2a;

    arch::machine_registers registers_{initial_registers(), arch::status_register{}};
    arch::machine_memory memory_{initial_memory()};

    std::shared_ptr<arch::debugger> debugger_{std::make_shared<arch::debugger>()};

    [[nodiscard]] static arch::registers initial_registers();
    [[nodiscard]] static arch::memory initial_memory();
  };

} // namespace yarisc::test

#endif
