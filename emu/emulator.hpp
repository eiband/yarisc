/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_EMU_EMULATOR_HPP
#define YARISC_EMU_EMULATOR_HPP

#include <yarisc/arch/feature_level.hpp>
#include <yarisc/arch/machine.hpp>

#include <filesystem>
#include <memory>

namespace yarisc::emu
{
  /**
   * @brief Interactive or non-interactive mode
   */
  enum class emulator_mode
  {
    /**
     * @brief Execute the emulator without user input and without debug viewer
     */
    unattended,

    /**
     * @brief Execute with prompt and debug viewer
     */
    interactive,
  };

  /**
   * @brief Emulator that executes a program on an emulated YaRISC CPU
   */
  class emulator final
  {
  public:
    static constexpr arch::feature_level default_level = arch::feature_level_latest;

    /**
     * @brief Constructor
     *
     * @param mode emulator mode
     * @param level feature level of the emulated machine
     */
    explicit emulator(arch::feature_level level = default_level, emulator_mode mode = emulator_mode::unattended);

    /**
     * @brief Constructor
     *
     * Does not load any image if the path is empty.
     *
     * @param image path to the image to be loaded into main memory
     * @param level feature level of the emulated machine
     * @param mode emulator mode
     */
    explicit emulator(
      const std::filesystem::path& image,
      arch::feature_level level = default_level,
      emulator_mode mode = emulator_mode::unattended);

    /**
     * @brief Executes the program at address zero
     *
     * @param mode execution mode normal or strict
     * @return true if a halt instruction was executed, false if a debugger breakpoint was hit
     */
    bool execute(arch::execution_mode mode = arch::execution_mode::normal)
    {
      return viewer_ ? viewer_->execute(machine_, mode) : machine_.execute(mode);
    }

  private:
    class viewer_base
    {
    public:
      explicit viewer_base(arch::debugger_ptr dbg) noexcept
        : debugger_{std::move(dbg)}
      {
      }

      viewer_base(const viewer_base& that) = delete;
      viewer_base(viewer_base&& that) = delete;

      virtual ~viewer_base() = default;

      viewer_base& operator=(const viewer_base& that) = delete;
      viewer_base& operator=(viewer_base&& that) = delete;

      [[nodsicard]] const arch::debugger_ptr& get_debugger() const noexcept
      {
        return debugger_;
      }

      virtual bool execute(arch::machine& m, arch::execution_mode mode) = 0;

    private:
      arch::debugger_ptr debugger_;
    };

    class viewer;

    std::unique_ptr<viewer_base> viewer_{};
    arch::machine machine_{};
  };

} // namespace yarisc::emu

#endif
