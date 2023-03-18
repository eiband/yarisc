/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DEBUGGER_HPP
#define YARISC_ARCH_DEBUGGER_HPP

#include <yarisc/arch/export.h>
#include <yarisc/arch/machine.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/output.hpp>

#include <iosfwd>
#include <string>
#include <string_view>

namespace yarisc::arch
{
  /**
   * @brief Debugger state used by the machine
   */
  class debugger final
  {
  public:
    /**
     * @brief Constructor
     */
    debugger() = default;

    debugger(const debugger& that) = delete;
    debugger(debugger&& that) = delete;

    /**
     * @brief Destructor
     */
    ~debugger() = default;

    debugger& operator=(const debugger& that) = delete;
    debugger& operator=(debugger&& that) = delete;

    /**
     * @brief Returns whether there was a panic
     */
    [[nodiscard]] bool panic() const noexcept
    {
      return panic_;
    }

    /**
     * @brief Returns the current message
     */
    [[nodiscard]] const std::string& message() const noexcept
    {
      return message_;
    }

    /**
     * @brief Resets the current message
     */
    void reset_message() noexcept
    {
      message_.clear();
    }

    /**
     * @brief Resets the current message and panic flag
     */
    void reset_panic() noexcept
    {
      reset_message();

      panic_ = false;
    }

    /**
     * @brief Stores a debug message if the debugger is not nullptr
     *
     * @param dbg pointer to debugger or nullptr
     * @param msg debug message
     * @return true if the debug message has been stored, false otherwise
     */
    YARISC_ARCH_EXPORT friend bool store_debug_message(debugger* dbg, std::string msg);

    /**
     * @brief Sets the panic flag if the debugger is not nullptr or throws a runtime error
     *
     * @param dbg pointer to debugger or nullptr
     * @param msg panic message
     */
    YARISC_ARCH_EXPORT friend void store_panic_or_throw(debugger* dbg, std::string msg);

  private:
    bool panic_{false};

    std::string message_;
  };

  /**
   * @brief Throws a runtime error exception
   *
   * @param msg panic message
   */
  [[noreturn]] YARISC_ARCH_EXPORT void throw_panic(const std::string& msg);

  /**
   * @brief A debugger view is used to output the debug state of the machine
   */
  struct debugger_view final
  {
    const debugger* debug{nullptr};

    registers_view current_registers{};
    memory_view current_memory{};

    registers_view previous_registers{};
    memory_view previous_memory{};

    std::string_view info{};
    std::string_view error{};
  };

  YARISC_ARCH_EXPORT void tag_invoke(output_t, std::ostream& os, const debugger_view& dbg, output_format fmt);

} // namespace yarisc::arch

#endif
