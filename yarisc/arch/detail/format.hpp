/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_FORMAT_HPP
#define YARISC_ARCH_DETAIL_FORMAT_HPP

#include <yarisc/arch/output.hpp>
#include <yarisc/arch/registers.hpp>
#include <yarisc/arch/types.hpp>
#include <yarisc/utils/color.hpp>
#include <yarisc/utils/ios.hpp>

#include <algorithm>
#include <concepts>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace yarisc::arch::detail
{
  template <typename T>
  class basic_format_t;

  template <typename Func, typename... Args>
    requires(
      std::invocable<const Func&, Args..., utils::color::plain_context&, std::ostream&> &&
      std::invocable<const Func&, Args..., utils::color::colored_context&, std::ostream&>)
  std::ostream& format_output(const Func& out, std::ostream& os, output_format fmt, Args&&... args)
  {
    const utils::ostream_guard guard{os};

    if ((fmt == output_format::colored) || ((fmt == output_format::console) && utils::color::supported()))
    {
      utils::color::colored_context ctx{};

      std::invoke(out, std::forward<Args>(args)..., ctx, os);

      os << utils::color::reset(ctx);
    }
    else
    {
      utils::color::plain_context ctx{};

      std::invoke(out, std::forward<Args>(args)..., ctx, os);
    }

    return os;
  }

  template <typename T, typename U>
  concept projectable = std::convertible_to<T, U> || std::same_as<T, diff<U>>;

  template <typename Func, typename T>
    requires std::invocable<const Func&, const T&>
  [[nodiscard]] auto proj(const Func& func, const T& value)
  {
    return std::invoke(func, value);
  }

  template <typename Func, typename T>
    requires std::invocable<const Func&, const T&>
  [[nodiscard]] auto proj(const Func& func, const diff<T>& d)
  {
    using proj_type = std::decay_t<decltype(proj(std::declval<const Func&>(), std::declval<const T&>()))>;

    return diff<proj_type>{proj(func, d.current), proj(func, d.previous)};
  }

  template <typename Func, typename T>
    requires std::invocable<const Func&, const T&>
  [[nodiscard]] auto proj_current(const Func& func, const T& value)
  {
    return proj(func, value);
  }

  template <typename Func, typename T>
    requires std::invocable<const Func&, const T&>
  [[nodiscard]] auto proj_current(const Func& func, const diff<T>& d)
  {
    return proj(func, d.current);
  }

  template <typename Func, typename T>
  [[nodiscard]] std::string output_as_string(const Func& out, const T& value)
  {
    utils::color::plain_context ctx{};

    std::stringstream os;
    std::invoke(out, value, ctx, os);

    return std::move(os).str();
  }

  template <typename T, std::convertible_to<T> U>
  [[nodiscard]] std::string output_as_string(const basic_format_t<T>&, const U& value)
  {
    std::stringstream os;
    os << value;

    return std::move(os).str();
  }

  template <std::convertible_to<std::string_view> T>
  [[nodiscard]] inline std::string_view output_as_string(const basic_format_t<std::string>&, const T& value)
  {
    return value;
  }

  template <std::convertible_to<std::string_view> T>
  [[nodiscard]] inline std::string_view output_as_string(const basic_format_t<std::string_view>&, const T& value)
  {
    return value;
  }

  struct output_char_pred
  {
    [[nodiscard]] bool operator()(char ch) const noexcept
    {
      return (ch >= 32) && (ch <= 126);
    }
  };

  inline std::ostream& output_char(std::ostream& os, char ch) noexcept
  {
    const output_char_pred pred{};

    return os << (pred(ch) ? ch : '.');
  }

  inline std::ostream& output_string(std::ostream& os, std::string_view str) noexcept
  {
    const output_char_pred pred{};

    if (std::ranges::all_of(str, pred))
    {
      os << str;
    }
    else
    {
      for (char ch : str)
        os << (pred(ch) ? ch : '.');
    }

    return os;
  }

  template <typename Func, typename T>
  struct highlight_diff_only : std::is_convertible<const T&, std::string_view>
  {
  };

  template <typename Func, typename T>
    requires requires { typename std::remove_cvref_t<Func>::highlight_diff_only; }
  struct highlight_diff_only<Func, T> : std::remove_cvref_t<Func>::highlight_diff_only
  {
  };

  template <typename Func, typename T>
  inline constexpr bool highlight_diff_only_v = highlight_diff_only<Func, T>::value;

  template <utils::color::context Ctx>
  void format_diff_impl(std::string_view current, std::string_view previous, Ctx& ctx, std::ostream& os, bool diff_only)
  {
    const std::string::size_type size = current.size();

    if (size > 0)
    {
      std::string::size_type i = 0;

      if (const std::string::size_type diff_size = std::min(size, previous.size()); diff_size > 0)
      {
        int previous_diff = -1;

        const auto highlight_diff = utils::color::bright_red(ctx);
        const auto highlight_same = diff_only ? utils::color::white(ctx) : utils::color::red(ctx);

        for (; i < diff_size; ++i)
        {
          const char ch = current[i];
          const int current_diff = (ch != previous[i]) ? 1 : 0;

          if (current_diff != previous_diff)
            os << (current_diff ? highlight_diff : highlight_same);

          output_char(os, ch);

          previous_diff = current_diff;
        }

        os << utils::color::reset(ctx);

        if (i < size)
          output_string(os, current.substr(i));
      }
      else
      {
        os << utils::color::bright_red(ctx) << current << utils::color::reset(ctx);
      }
    }
  }

  template <typename Func, typename T, utils::color::context Ctx>
  void format_diff(const Func& out, const T& current, const T& previous, Ctx& ctx, std::ostream& os)
  {
    if (current == previous)
    {
      std::invoke(out, current, ctx, os);

      return;
    }

    format_diff_impl(
      output_as_string(out, current), output_as_string(out, previous), ctx, os, highlight_diff_only_v<Func, T>);
  }

  template <typename Func, typename T, utils::color::context Ctx>
  void format_proj(const Func& out, const diff<T>& d, Ctx& ctx, std::ostream& os)
  {
    if constexpr (utils::color::always_enabled<Ctx>)
      format_diff(out, d.current, d.previous, ctx, os);
    else if constexpr (utils::color::never_enabled<Ctx>)
      std::invoke(out, d.current, ctx, os);
    else if (ctx.enabled())
      format_diff(out, d.current, d.previous, ctx, os);
    else
      std::invoke(out, d.current, ctx, os);
  }

  template <typename Func, typename T, utils::color::context Ctx>
  class bound_output final
  {
  public:
    bound_output(Func out, T value, Ctx& ctx)
      : output_(std::move(out))
      , value_(std::move(value))
      , context_{&ctx}
    {
    }

  private:
    Func output_;
    T value_;
    Ctx* context_;

    friend std::ostream& operator<<(std::ostream& os, const bound_output& b)
    {
      std::invoke(b.output_, b.value_, *b.context_, os);

      return os;
    }
  };

  template <typename Func, typename T, utils::color::context Ctx>
  [[nodiscard]] auto bind_output(Func out, T value, Ctx& ctx)
  {
    return bound_output{std::move(out), std::move(value), ctx};
  }

  template <typename Derived, typename T>
  class bind_output_fn
  {
  public:
    template <projectable<T> U, utils::color::context Ctx>
    [[nodiscard]] auto operator()(const U& value, Ctx& ctx) const
    {
      return bind_output(static_cast<const Derived&>(*this), value, ctx);
    }

    template <projectable<T> U, utils::color::context Ctx>
    void operator()(const U& value, Ctx& ctx, std::ostream& os) const
    {
      put_impl(value, ctx, os);
    }

  private:
    template <projectable<T> U, utils::color::context Ctx>
    auto put_impl(const U& value, Ctx& ctx, std::ostream& os) const
      -> decltype(std::declval<const Derived&>().put(value, ctx, os))
    {
      return static_cast<const Derived&>(*this).put(value, ctx, os);
    }

    template <typename U, typename Ctx>
    auto put_impl(const U& value, Ctx& ctx, std::ostream& os) const
      -> decltype(format_proj(std::declval<const Derived&>(), value, ctx, os))
    {
      return format_proj(static_cast<const Derived&>(*this), value, ctx, os);
    }
  };

  template <typename T>
  class basic_format_t : public bind_output_fn<basic_format_t<T>, T>
  {
  public:
    explicit basic_format_t() = default;

  private:
    friend bind_output_fn<basic_format_t, T>;

    template <utils::color::context Ctx>
    void put(const T& value, Ctx&, std::ostream& os) const
    {
      if constexpr (std::is_convertible_v<T, std::string_view>)
        output_string(os, value);
      else
        os << value;
    }
  };

  template <typename T>
  inline constexpr basic_format_t<T> basic_format{};
  inline constexpr basic_format_t<std::string_view> string_format{};

} // namespace yarisc::arch::detail

#endif
