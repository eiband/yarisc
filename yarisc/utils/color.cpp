/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/utils/color.hpp>

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace yarisc::utils::color
{
  namespace
  {
#if defined(_WIN32)

    [[nodiscard]] bool try_enable_color(DWORD std_handle) noexcept
    {
      const HANDLE handle = ::GetStdHandle(std_handle);

      if ((handle != NULL) && (handle != INVALID_HANDLE_VALUE))
      {
        DWORD mode{};

        if (::GetConsoleMode(handle, &mode))
        {
          const DWORD new_mode = mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

          if ((mode == new_mode) || ::SetConsoleMode(handle, new_mode))
            return true;
        }
      }

      return false;
    }

    [[nodiscard]] bool try_enable_color() noexcept
    {
      return try_enable_color(STD_OUTPUT_HANDLE) && try_enable_color(STD_ERROR_HANDLE);
    }

#endif

  } // namespace

  namespace detail
  {
    constinit std::atomic<color_support> glob_color_support{color_support::unknown};

    bool enable_color() noexcept
    {
      static const bool supported = []()
      {
        const bool enabled = try_enable_color();

        glob_color_support.store(enabled ? color_support::yes : color_support::no, std::memory_order::release);

        return enabled;
      }();

      return supported;
    }

  } // namespace detail

} // namespace yarisc::utils::color
