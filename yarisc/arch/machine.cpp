/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/arch/machine.hpp>

#include <yarisc/arch/detail/execution.hpp>

#include <cassert>
#include <cstring>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace yarisc::arch
{
  namespace
  {
    template <typename Profile, typename Func, typename... Args>
    decltype(auto) switch_policy(debugger* dbg, execution_mode mode, Func&& func, Args&&... args)
    {
      if (mode == execution_mode::strict)
      {
        if (dbg)
        {
          return std::forward<Func>(func)(
            detail::make_execution_policy<Profile>(
              detail::debug_execution_policy{dbg}, detail::strict_execution_policy{}),
            std::forward<Args>(args)...);
        }
        else
        {
          return std::forward<Func>(func)(
            detail::make_execution_policy<Profile>(
              detail::noop_debug_execution_policy{}, detail::strict_execution_policy{}),
            std::forward<Args>(args)...);
        }
      }
      else
      {
        if (dbg)
        {
          return std::forward<Func>(func)(
            detail::make_execution_policy<Profile>(
              detail::debug_execution_policy{dbg}, detail::noop_strict_execution_policy{}),
            std::forward<Args>(args)...);
        }
        else
        {
          return std::forward<Func>(func)(
            detail::make_execution_policy<Profile>(
              detail::noop_debug_execution_policy{}, detail::noop_strict_execution_policy{}),
            std::forward<Args>(args)...);
        }
      }
    }

    template <typename Func, typename... Args>
    decltype(auto) switch_level(debugger* dbg, feature_level level, execution_mode mode, Func&& func, Args&&... args)
    {
      switch (level)
      {
      case feature_level::min:
        return switch_policy<machine_profile<feature_level::min>>(
          dbg, mode, std::forward<Func>(func), std::forward<Args>(args)...);
      case feature_level::v1:
        return switch_policy<machine_profile<feature_level::v1>>(
          dbg, mode, std::forward<Func>(func), std::forward<Args>(args)...);
      default:
        throw std::runtime_error{
          "Invalid feature level " + std::to_string(static_cast<std::underlying_type_t<feature_level>>(level))};
      }
    }

    struct execute_func final
    {
      execute_func() = default;

      template <typename Policy>
      [[nodiscard]] bool operator()(Policy policy, detail::machine_data& data)
      {
        detail::execute_result result{};

        while (result.keep_going) [[likely]]
          result = detail::execute_instruction(policy, data.state.reg, data.mem);

        return !result.breakpoint;
      }

      template <typename Policy>
      [[nodiscard]] std::pair<bool, std::uint64_t> operator()(
        Policy policy, detail::machine_data& data, std::uint64_t steps)
      {
        detail::execute_result result{};

        std::uint64_t s = 0;
        bool compute = (steps > 0);

        while (compute) [[likely]]
        {
          result = detail::execute_instruction(policy, data.state.reg, data.mem);

          compute = result.keep_going && (steps > ++s);
        }

        return {!result.breakpoint && !result.keep_going, s};
      }
    };

  } // namespace

  machine::machine(debugger_ptr dbg, feature_level level)
    : level_{level}
    , debugger_{std::move(dbg)}
  {
    data_.state.debug = debugger_.get();
  }

  void machine::load(const std::filesystem::path& image)
  {
    std::ifstream fs{image, std::ios::binary};

    // Safety check
    if (!fs.is_open())
      throw std::runtime_error{"could not open image file"};

    fs.unsetf(std::ios::skipws);
    fs.seekg(0, std::ios::end);

    const auto sz = static_cast<memory::size_type>(fs.tellg());

    if (sz > data_.mem.main.size())
      throw std::out_of_range{"the image file is too big"};

    if (sz > 0)
    {
      fs.seekg(0, std::ios::beg);

      std::vector<unsigned char> buf;
      buf.reserve(sz);

      // Read the entire file
      buf.insert(begin(buf), std::istream_iterator<unsigned char>{fs}, std::istream_iterator<unsigned char>{});

      // Safety check
      if (buf.size() != sz)
        throw std::out_of_range{"unexpected image size"};

      assert(buf.size() <= data_.mem.main.size());

      std::memcpy(data_.mem.main.data(), buf.data(), buf.size());
    }
  }

  bool machine::execute(execution_mode mode)
  {
    return switch_level(debugger_.get(), level_, mode, execute_func{}, data_);
  }

  std::pair<bool, std::uint64_t> machine::execute(std::uint64_t steps, execution_mode mode)
  {
    return switch_level(debugger_.get(), level_, mode, execute_func{}, data_, steps);
  }

} // namespace yarisc::arch
