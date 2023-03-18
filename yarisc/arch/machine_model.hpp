/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_MACHINE_MODEL_HPP
#define YARISC_ARCH_MACHINE_MODEL_HPP

#include <yarisc/arch/export.h>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/output.hpp>
#include <yarisc/arch/registers.hpp>

#include <array>
#include <iosfwd>
#include <optional>

namespace yarisc::arch
{
  /**
   * @brief Machine memory
   *
   * This struct contains all CPU external memory.
   */
  struct machine_memory final
  {
    /**
     * @brief Main memory
     */
    memory main{};

    [[nodiscard]] bool operator==(const machine_memory& that) const noexcept = default;
  };

  /**
   * @brief Machine registers
   *
   * This struct contains all CPU internal registers.
   */
  struct machine_registers final
  {
    /**
     * @brief Named registers `r0`, `r1`, `r2`, `r3`, `r4`, `r5`, `sp`, `ip`
     */
    registers named{};

    /**
     * @brief Status register
     */
    status_register status{};

    [[nodiscard]] bool operator==(const machine_registers& that) const noexcept = default;
  };

  /**
   * @brief A registers view is used to output the registers
   */
  struct registers_view final
  {
    /**
     * @brief Copy of the named registers `r0`, `r1`, `r2`, `r3`, `r4`, `r5`, `sp`, `ip`
     */
    registers named{};

    /**
     * @brief Copy of the status register
     */
    status_register status{};

    /**
     * @brief Current instruction words
     */
    std::optional<std::array<word_t, 2>> instruction{};

    /**
     * @brief Constructor
     */
    registers_view() = default;

    /**
     * @brief Constructor
     *
     * Creates a register view from a copy of the given machine registers.
     *
     * @param reg current machine registers
     */
    explicit registers_view(const machine_registers& reg) noexcept
      : named{reg.named}
      , status{reg.status}
    {
    }

    /**
     * @brief Constructor
     *
     * Creates a register view from a copy of the given machine registers and a copy of the current instruction.
     *
     * @param reg current machine registers
     * @param mem current machine memory to fetch the current instruction
     */
    registers_view(const machine_registers& reg, const machine_memory& mem) noexcept
      : named{reg.named}
      , status{reg.status}
    {
      if (!mem.main.empty() && detail::is_aligned(reg.named.ip()))
      {
        const auto size = mem.main.size();
        const auto address = static_cast<memory::size_type>(reg.named.ip());

        instruction = std::array<word_t, 2>{{
          mem.main.load(static_cast<address_t>(address % size)),
          mem.main.load(static_cast<address_t>((address + sizeof(word_t)) % size)),
        }};
      }
    }
  };

  YARISC_ARCH_EXPORT void tag_invoke(output_t, std::ostream& os, const registers_view& reg, output_format fmt);
  YARISC_ARCH_EXPORT void tag_invoke(output_t, std::ostream& os, const diff<registers_view>& reg, output_format fmt);

} // namespace yarisc::arch

#endif
