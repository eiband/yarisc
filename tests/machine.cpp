/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <tests/machine.hpp>

#include <yarisc/arch/assembly.hpp>
#include <yarisc/arch/detail/execution.hpp>
#include <yarisc/arch/machine_profile.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace yarisc::test
{
  machine::machine() = default;

  machine::machine(arch::word_t word)
  {
    const auto ip = static_cast<size_type>(registers_.named.ip());

    store(ip, word);
  }

  machine::machine(arch::word_t word0, arch::word_t word1)
  {
    const auto ip = static_cast<size_type>(registers_.named.ip());

    store(ip, word0);
    store(ip + sizeof(arch::word_t), word1);
  }

  arch::word_t machine::load(size_type off) const
  {
    if (!arch::detail::is_aligned(off)) [[unlikely]]
      throw std::runtime_error{"unaligned address in load"};

    return (off < memory_.main.size()) ? memory_.main.load(static_cast<arch::address_t>(off)) : 0xffff;
  }

  bool machine::store(size_type off, arch::word_t word)
  {
    if (!arch::detail::is_aligned(off)) [[unlikely]]
      throw std::runtime_error{"unaligned address in store"};

    if (off >= memory_.main.size()) [[unlikely]]
      return false;

    memory_.main.store(static_cast<arch::address_t>(off), word);

    return true;
  }

  bool machine::execute_instruction(bool throw_on_breakpoint)
  {
    auto policy = arch::detail::make_execution_policy<arch::machine_profile<arch::feature_level_latest>>(
      arch::detail::debug_execution_policy{debugger_.get()}, arch::detail::strict_execution_policy{});

    const auto [keep_going, breakpoint] = arch::detail::execute_instruction(policy, registers_, memory_);

    if (debugger_ && debugger_->panic())
      throw std::runtime_error{debugger_->message()};

    if (throw_on_breakpoint && breakpoint)
      throw std::runtime_error{(debugger_ && !debugger_->message().empty()) ? debugger_->message() : "breakpoint"};

    return keep_going;
  }

  std::string machine::disassemble_instruction(int expected_words) const
  {
    const auto ip = static_cast<size_type>(registers_.named.ip());

    arch::disassembly result = arch::disassemble(load(ip), load(ip + sizeof(arch::word_t)));

    if (result.words == 0)
      throw std::runtime_error{result.text};
    if (result.words != expected_words)
      throw std::runtime_error{"wrong number of instruction words"};

    return result.text;
  }

  arch::registers machine::initial_registers()
  {
    arch::registers named{};
    named.set_r0(0xa9b4);
    named.set_r1(0x1172);
    named.set_r2(0x30cc);
    named.set_r3(0x6ce5);
    named.set_r4(0x8bd3);
    named.set_r5(0xf196);
    named.set_sp(initial_sp);
    named.set_ip(initial_ip);

    return named;
  }

  arch::memory machine::initial_memory()
  {
    arch::memory mem{128 /* bytes */};

    std::uint8_t value = 0;
    std::ranges::generate(mem, [&value]() { return static_cast<std::byte>(value++); });

    return mem;
  }

} // namespace yarisc::test
