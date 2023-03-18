/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_MACHINE_HPP
#define YARISC_ARCH_MACHINE_HPP

#include <yarisc/arch/export.h>
#include <yarisc/arch/feature_level.hpp>
#include <yarisc/arch/machine_model.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/registers.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <utility>

namespace yarisc::arch
{
  class debugger;

  using debugger_ptr = std::shared_ptr<debugger>;

  /**
   * @brief Execution mode
   */
  enum class execution_mode
  {
    /**
     * @brief No additional checks
     */
    normal,

    /**
     * @brief Adds runtime checks
     *
     * Strict execution checks that unassigned instruction bits have the value zero and that loads and stores are
     * word-aligned.
     */
    strict,
  };

  /**
   * @brief Machine state
   *
   * This struct contains all state, such as all registers and watch entries for debugging, except for memory and
   * devices. The state is copied from one step to another in single-step debugging. For debugging memory typically only
   * a small section is displayed, therfore we only keep track of this area instead of copying the whole memory.
   */
  struct machine_state final
  {
    machine_registers reg{};

    /**
     * @warning
     * For simplicity we store a pointer to an optional debugger in the machine state. The lifetime of the debugger must
     * exceed all copies of the machine state. This is the only reference semantics part of the machine. Given that
     * there is typically just one debugger for the lifetime of the whole program this is acceptable for now.
     */
    const debugger* debug{nullptr};
  };

  namespace detail
  {
    struct machine_data final
    {
      machine_state state{initial_state()};
      machine_memory mem{};

      machine_data() = default;

      machine_data(const machine_data& that) = default;

      machine_data(machine_data&& that) noexcept
        : state{std::exchange(that.state, initial_state())}
        , mem{std::move(that.mem)}
      {
      }

      ~machine_data() = default;

      machine_data& operator=(const machine_data& that) = default;

      machine_data& operator=(machine_data&& that) noexcept
      {
        if (this != &that)
        {
          state = std::exchange(that.state, initial_state());
          mem = std::move(that.mem);
        }

        return *this;
      }

      void reset(debugger* dbg) noexcept
      {
        state = initial_state();
        state.debug = dbg;

        mem.main.clear();
      }

      [[nodiscard]] static machine_state initial_state() noexcept
      {
        return {};
      }
    };

  } // namespace detail

  /**
   * @brief Full description of the machine
   *
   * The machine comprises of the CPU internal machine state and main memory.
   */
  class machine final
  {
  public:
    /**
     * @brief Constructor
     *
     * Constructs the machine in the initial state.
     */
    machine() = default;

    /**
     * @brief Constructor
     *
     * Constructs the machine in the initial state.
     *
     * @param level feature level of the machine
     */
    explicit machine(feature_level level)
      : level_{level}
    {
    }

    /**
     * @brief Constructor
     *
     * Constructs the machine in the initial state with a debugger.
     *
     * @param dbg optional pointer to a debugger
     * @param level feature level of the machine
     */
    YARISC_ARCH_EXPORT explicit machine(debugger_ptr dbg, feature_level level = feature_level_latest);

    /**
     * @brief Returns the state of the machine
     */
    [[nodiscard]] const machine_state& state() const noexcept
    {
      return data_.state;
    }

    /**
     * @brief Returns the state of the machine for output
     */
    [[nodiscard]] registers_view state_view() const noexcept
    {
      return {data_.state.reg, data_.mem};
    }

    /**
     * @brief Returns the main memory of the machine
     */
    [[nodiscard]] memory& main_memory() noexcept
    {
      return data_.mem.main;
    }

    /**
     * @brief Returns the main memory of the machine
     */
    [[nodiscard]] const memory& main_memory() const noexcept
    {
      return data_.mem.main;
    }

    /**
     * @brief Returns a view into main memory of the machine
     *
     * @param off byte address into the main memory
     * @param sz maximum size in bytes of the returned view
     * @param main memory view at the offset
     */
    [[nodiscard]] memory_view main_memory(memory::size_type off, memory::size_type sz = memory::npos) const noexcept
    {
      return data_.mem.main.sub(off, sz, &data_.state);
    }

    /**
     * @brief Loads an image into main memory
     *
     * @note
     * If the image is smaller than main memory then only the bytes from the image will be written. It is recommended to
     * reset the machine first.
     *
     * @param image path to the image to be loaded into main memory
     */
    YARISC_ARCH_EXPORT void load(const std::filesystem::path& image);

    /**
     * @brief Executes until a halt instruction is executed or a debugger breakpoint is hit
     *
     * @param mode execution mode normal or strict
     * @return true if halted, false if a debugger breakpoint was hit
     */
    YARISC_ARCH_EXPORT bool execute(execution_mode mode = execution_mode::normal);

    /**
     * @brief Executes a given number of steps
     *
     * @param number of steps to execute
     * @param mode execution mode normal or strict
     * @return a boolean whether the machine was halted and the number of executed steps
     */
    YARISC_ARCH_EXPORT std::pair<bool, std::uint64_t> execute(
      std::uint64_t steps, execution_mode mode = execution_mode::normal);

    /**
     * @brief Resets the machine to initial state
     *
     * This function keeps the debugger.
     */
    void reset() noexcept
    {
      data_.reset(debugger_.get());
    }

    /**
     * @brief Swaps with another machine
     *
     * @param that other machine to swap with
     */
    void swap(machine& that) noexcept
    {
      using std::swap;

      swap(data_, that.data_);
      swap(level_, that.level_);
      swap(debugger_, that.debugger_);
    }

  private:
    detail::machine_data data_;
    feature_level level_{feature_level_latest};

    debugger_ptr debugger_;
  };

  /**
   * @brief Swaps two machines
   *
   * @param lhs first machine
   * @param rhs second machine
   */
  inline void swap(machine& lhs, machine& rhs) noexcept
  {
    lhs.swap(rhs);
  }

} // namespace yarisc::arch

#endif
