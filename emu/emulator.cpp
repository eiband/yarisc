/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <emu/emulator.hpp>

#include <yarisc/arch/debugger.hpp>
#include <yarisc/arch/memory.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace yarisc::emu
{
  namespace
  {
    template <utils::color::context Ctx>
    [[nodiscard]] auto reset_cursor(Ctx& ctx, bool clear_screen)
    {
      using namespace std::string_view_literals;

      return utils::color::manip(ctx, clear_screen ? "\033[H\033[2J"sv : "\033[H"sv);
    }

  } // namespace

  class emulator::viewer : public viewer_base
  {
  public:
    viewer()
      : viewer_base{std::make_shared<arch::debugger>()}
    {
    }

    bool execute(arch::machine& m, arch::execution_mode mode) override
    {
      const arch::debugger_ptr dbg = get_debugger();

      session_->set_clear_display();

      for (;;)
      {
        utils::color::dynamic_context ctx;

        session_->display(m, ctx, dbg.get());

        const auto [exit, steps] = user_prompt(m, ctx, dbg.get());

        if (exit)
        {
          session_->display(m, ctx, dbg.get());

          break;
        }

        if (steps)
          session_->execute(m, mode, dbg.get(), *steps);
        else
          session_->execute(m, mode, dbg.get());

        session_->update_messages(dbg.get());
      }

      // The session is always exited by the user. Any breakpoint is interpreted as handled by the user, so we always
      // return true here.

      return true;
    }

  private:
    static constexpr std::string_view info_message = "Type 'h' for a list of commands";
    static constexpr std::string_view help_message =
      "Commands: h: help, hh: more help, e: exit, r: reset, l <path>: load image";
    static constexpr std::string_view more_help_message = "Commands: s: single step, x: execute";

    static constexpr std::string_view finished_message = "Program has finished";

    class session final
    {
    public:
      session() = default;

      session(const session& that) = delete;
      session(session&& that) = delete;

      ~session() = default;

      session& operator=(const session& that) = delete;
      session& operator=(session&& that) = delete;

      void display(const arch::machine& m, utils::color::dynamic_context& ctx, const arch::debugger* dbg)
      {
        const arch::registers_view current_registers = m.state_view();
        const arch::memory_view current_memory =
          m.main_memory(static_cast<size_type>(memory_debug_base_), memory_debug_size_);

        const arch::registers_view previous_registers{previous_state_.reg};
        const arch::memory_view previous_memory{
          previous_memory_.get(), memory_debug_size_, memory_debug_base_, &previous_state_};

        const std::string_view info = info_message_.empty() && error_message_.empty() ? info_message : info_message_;
        const std::string_view error = error_message_;

        const arch::debugger_view view{
          dbg, current_registers, current_memory, previous_registers, previous_memory, info, error};

        // It is faster to accumulate everyting in a string first and then output the whole string at once
        std::ostringstream oss;
        oss << reset_cursor(ctx, clear_display_) << utils::color::reset(ctx) << view;

        assert(!ctx.dirty());

        std::cout << std::move(oss).str();

        clear_display_ = false;
        message_displayed_ = true;
      }

      void set_clear_display() noexcept
      {
        clear_display_ = true;
      }

      void execute(arch::machine& m, arch::execution_mode mode, const arch::debugger* dbg)
      {
        if (check_execute(m))
        {
          finished_ = m.execute(mode) || (dbg && dbg->panic());
          previous_steps_ = 0;
        }
      }

      void execute(arch::machine& m, arch::execution_mode mode, const arch::debugger* dbg, std::uint64_t steps)
      {
        if ((steps > 0) && check_execute(m))
        {
          const auto result = m.execute(steps, mode);

          finished_ = result.first || (dbg && dbg->panic());
          previous_steps_ = result.second;
        }
      }

      template <typename Arg>
      void set_info_message(Arg&& msg)
      {
        info_message_ = std::forward<Arg>(msg);
        message_displayed_ = false;
      }

      template <typename Arg>
      void set_error_message(Arg&& msg)
      {
        error_message_ = std::forward<Arg>(msg);
        message_displayed_ = false;
      }

      void update_state(const arch::machine& m)
      {
        previous_state_ = m.state();

        std::memcpy(previous_memory_.get(), m.main_memory().data() + memory_debug_base_, memory_debug_size_);
      }

      void update_messages(arch::debugger* dbg)
      {
        if (message_displayed_ || (info_message_.empty() && error_message_.empty()))
        {
          if (dbg)
          {
            if (dbg->panic())
            {
              // Don't reset the panic message
              error_message_ = dbg->message();
            }
            else if (info_message_.empty())
            {
              info_message_ = dbg->message();

              dbg->reset_message();
            }
          }

          if (finished_ && info_message_.empty() && error_message_.empty())
            info_message_ = finished_message;
        }
      }

      void reset_messages() noexcept
      {
        info_message_.clear();
        error_message_.clear();
      }

    private:
      bool finished_{false};
      bool clear_display_{false};
      bool message_displayed_{false};

      std::string info_message_{};
      std::string error_message_{};

      using size_type = arch::memory::size_type;

      arch::address_t memory_debug_base_{0};
      size_type memory_debug_size_{256};

      std::uint64_t previous_steps_{0};
      arch::machine_state previous_state_{};
      std::unique_ptr<arch::memory::value_type[]> previous_memory_{
        std::make_unique<arch::memory::value_type[]>(memory_debug_size_)};

      [[nodiscard]] bool check_execute(const arch::machine& m)
      {
        if (finished_)
        {
          error_message_ = finished_message;

          return false;
        }

        reset_messages();
        update_state(m);

        return true;
      }
    };

    std::unique_ptr<session> session_{std::make_unique<session>()};

    bool already_clear_{true};

    [[nodiscard]] std::pair<bool, std::optional<std::uint64_t>> user_prompt(
      arch::machine& m, utils::color::dynamic_context& ctx, arch::debugger* dbg)
    {
      using namespace std::string_view_literals;

      constexpr auto prompt = "$ "sv;

      assert(!ctx.dirty());

      bool exit = false;
      std::optional<std::uint64_t> steps{0};

      std::cout << '\n' << prompt;

      std::string command;
      std::getline(std::cin, command);

      session_->reset_messages();

      if (command.empty())
      {
        if (!already_clear_)
        {
          session_->set_clear_display();
          already_clear_ = true;
        }
      }
      else
      {
        // Heuristic when it should be fine to just go up one line and overwrite the user input. We don't know how to
        // backtrace N characters with wrap around at the left side of the display.

        if (ctx.enabled() && (command.size() < 60))
        {
          std::cout << "\033[A"sv << prompt << std::string(command.size(), ' ') << '\n';
          already_clear_ = false;
        }
        else
        {
          session_->set_clear_display();
          already_clear_ = true;
        }

        if (command == "h")
          session_->set_info_message(help_message);
        else if (command == "hh")
          session_->set_info_message(more_help_message);
        else if (command == "e")
          exit = true;
        else if (command == "s")
          steps = 1;
        else if (command == "x")
          steps.reset();
        else if (command == "r")
          reset_machine(m, dbg);
        else if (command == "l")
          session_->set_error_message("Load command expects an image file path: l path/to/image");
        else if (command.starts_with("l "))
          reset_machine(m, dbg, command.substr(2));
        else
          session_->set_error_message("Unknown command: " + command);
      }

      assert(!ctx.dirty());

      return {exit, steps};
    }

    void reset_machine(
      arch::machine& m, arch::debugger* dbg, const std::optional<std::filesystem::path>& image = std::nullopt)
    {
      using namespace std::string_literals;

      session_ = std::make_unique<session>();

      m.reset();

      if (dbg)
        dbg->reset_panic();

      if (image)
      {
        if (!image->empty())
        {
          try
          {
            m.load(*image);
          }
          catch (const std::exception& ex)
          {
            session_->set_error_message("Error: "s + ex.what());
          }

          session_->update_state(m);
          session_->set_info_message("Image " + image->string() + " loaded successfully");
        }
        else
        {
          session_->set_error_message("No image file given");
        }
      }
      else
      {
        session_->set_info_message("Reset to initial state");
      }
    }
  };

  emulator::emulator(arch::feature_level level, emulator_mode mode)
    : viewer_{(mode == emulator_mode::interactive) ? std::make_unique<viewer>() : nullptr}
    , machine_{viewer_ ? viewer_->get_debugger() : nullptr, level}
  {
  }

  emulator::emulator(const std::filesystem::path& image, arch::feature_level level, emulator_mode mode)
    : emulator{level, mode}
  {
    if (!image.empty())
      machine_.load(image);
  }

} // namespace yarisc::emu
