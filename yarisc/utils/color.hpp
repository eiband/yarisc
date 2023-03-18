/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_UTILS_COLOR_HPP
#define YARISC_UTILS_COLOR_HPP

#include <yarisc/utils/export.h>

#include <atomic>
#include <concepts>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace yarisc::utils::color
{
  namespace detail
  {
    class color_never;
    class color_always;
    class color_conditional;
    class color_reset;

    enum class color_support : int
    {
      unknown = 0,
      yes,
      no
    };

    YARISC_UTILS_EXPORT extern std::atomic<color_support> glob_color_support;

    [[nodiscard]] YARISC_UTILS_EXPORT bool enable_color() noexcept;

    struct context_base
    {
      bool dirty_{false};
    };

  } // namespace detail

  /**
   * @brief Returns of console colors are supported
   *
   * @return true if console colors are supported, false otherwise
   */
  inline bool supported() noexcept
  {
    const auto support = detail::glob_color_support.load(std::memory_order::acquire);

    if (support != detail::color_support::unknown)
      return (support == detail::color_support::yes);

    return detail::enable_color();
  }

  /**
   * @brief Context that has color always disabled
   */
  class plain_context final
  {
  public:
    constexpr plain_context() noexcept = default;

    plain_context(const plain_context& that) = delete;
    plain_context(plain_context&& that) = default;

    ~plain_context() = default;

    plain_context& operator=(const plain_context& that) = delete;
    plain_context& operator=(plain_context&& that) = delete;

    /**
     * @brief Returns always false
     */
    [[nodiscard]] static constexpr bool enabled() noexcept
    {
      return false;
    }
  };

  /**
   * @brief Context that has color always enabled
   */
  class colored_context final : private detail::context_base
  {
  public:
    constexpr colored_context() noexcept = default;

    colored_context(const colored_context& that) = delete;

    /**
     * @brief Move constructor
     *
     * Takes over the dirty flag.
     *
     * @param that context to move
     */
    colored_context(colored_context&& that) noexcept
      : detail::context_base{that.reset_dirty()}
    {
    }

    ~colored_context() = default;

    colored_context& operator=(const colored_context& that) = delete;
    colored_context& operator=(colored_context&& that) = delete;

    /**
     * @brief Returns always true
     */
    [[nodiscard]] static constexpr bool enabled() noexcept
    {
      return true;
    }

    /**
     * @brief Returns whether color needs to be reset
     */
    [[nodiscard]] bool dirty() const noexcept
    {
      return dirty_;
    }

  private:
    friend detail::color_always;
    friend detail::color_reset;

    friend class dynamic_context;

    [[nodiscard]] bool reset_dirty() noexcept
    {
      return std::exchange(dirty_, false);
    }
  };

  /**
   * @brief Context that determines at runtime whether color is enabled
   */
  class dynamic_context final : private detail::context_base
  {
  public:
    /**
     * @brief Constructor
     *
     * Checks whether console colors are supported and sets the enabled state accordingly.
     */
    dynamic_context() noexcept = default;

    dynamic_context(const dynamic_context& that) = delete;

    /**
     * @brief Move constructor
     *
     * Takes over the dirty flag.
     *
     * @param that context to move
     */
    dynamic_context(dynamic_context&& that) noexcept
      : detail::context_base{that.reset_dirty()}
      , enabled_{that.enabled_}
    {
    }

    /**
     * @brief Constructor
     *
     * Conversion from a plain context. This context will be disabled.
     */
    dynamic_context(plain_context&&) noexcept
      : detail::context_base{}
      , enabled_{false}
    {
    }

    /**
     * @brief Constructor
     *
     * Conversion from a colored context. This context will be enabled.
     */
    dynamic_context(colored_context&& ctx) noexcept
      : detail::context_base{ctx.reset_dirty()}
      , enabled_{true}
    {
    }

    ~dynamic_context() = default;

    dynamic_context& operator=(const dynamic_context& that) = delete;
    dynamic_context& operator=(dynamic_context&& that) = delete;

    /**
     * @brief Returns whether color is enabled in this context
     */
    [[nodiscard]] bool enabled() const noexcept
    {
      return enabled_;
    }

    /**
     * @brief Returns whether color needs to be reset
     */
    [[nodiscard]] bool dirty() const noexcept
    {
      return dirty_;
    }

  private:
    friend detail::color_conditional;
    friend detail::color_reset;

    bool enabled_{supported()};

    [[nodiscard]] bool reset_dirty() noexcept
    {
      return std::exchange(dirty_, false);
    }
  };

  namespace detail
  {
    class color_never final
    {
    public:
      template <typename... Args>
      explicit color_never(plain_context&, Args&&...)
      {
      }

    private:
      friend std::ostream& operator<<(std::ostream& os, const color_never&) noexcept
      {
        return os;
      }
    };

    class color_always final
    {
    public:
      explicit color_always(colored_context& ctx, std::string_view sequence) noexcept
        : context_{&ctx}
        , sequence_{sequence}
      {
      }

    private:
      colored_context* context_;
      std::string_view sequence_;

      void put(std::ostream& os) const
      {
        os << sequence_;

        context_->dirty_ = true;
      }

      friend std::ostream& operator<<(std::ostream& os, const color_always& clr)
      {
        clr.put(os);

        return os;
      }
    };

    class color_conditional final
    {
    public:
      explicit color_conditional(dynamic_context& ctx, std::string_view sequence) noexcept
        : context_{&ctx}
        , sequence_{sequence}
      {
      }

    private:
      dynamic_context* context_;
      std::string_view sequence_;

      void put(std::ostream& os) const
      {
        if (context_->enabled_)
        {
          os << sequence_;

          context_->dirty_ = true;
        }
      }

      friend std::ostream& operator<<(std::ostream& os, const color_conditional& clr)
      {
        clr.put(os);

        return os;
      }
    };

    class color_reset final
    {
    public:
      explicit color_reset(dynamic_context& ctx) noexcept
        : context_{&ctx}
      {
      }

      explicit color_reset(colored_context& ctx) noexcept
        : context_{&ctx}
      {
      }

    private:
      context_base* context_;

      void put(std::ostream& os) const
      {
        using namespace std::string_view_literals;

        if (context_->dirty_)
        {
          os << "\033[0m"sv;

          context_->dirty_ = false;
        }
      }

      friend std::ostream& operator<<(std::ostream& os, const color_reset& clr)
      {
        clr.put(os);

        return os;
      }
    };

    template <typename T>
    struct context_traits
    {
      static constexpr bool valid = false;
    };

    template <>
    struct context_traits<plain_context>
    {
      static constexpr bool valid = true;

      using manip_type = color_never;
      using manip_reset_type = color_never;
    };

    template <>
    struct context_traits<colored_context>
    {
      static constexpr bool valid = true;

      using manip_type = color_always;
      using manip_reset_type = color_reset;
    };

    template <>
    struct context_traits<dynamic_context>
    {
      static constexpr bool valid = true;

      using manip_type = color_conditional;
      using manip_reset_type = color_reset;
    };

  } // namespace detail

  /**
   * @brief Concept that returns whether a type is any of the well known color contexts
   */
  template <typename T>
  concept context = detail::context_traits<std::remove_cvref_t<T>>::valid;

  // clang-format off

  /**
   * @brief Concept that returns whether a color context has color always enabled
   */
  template <typename T>
  concept always_enabled =
    context<T>
    && requires {
         { std::bool_constant<std::remove_cvref_t<T>::enabled()>{} } -> std::same_as<std::true_type>;
       };

  /**
   * @brief Concept that returns whether a color context has color always disabled
   */
  template <typename T>
  concept never_enabled =
    context<T>
    && requires {
         { std::bool_constant<std::remove_cvref_t<T>::enabled()>{} } -> std::same_as<std::false_type>;
       };

  // clang-format on

  inline constexpr std::string_view red_foreground_seq = "\033[31m";
  inline constexpr std::string_view white_foreground_seq = "\033[37m";
  inline constexpr std::string_view bright_red_foreground_seq = "\033[91m";
  inline constexpr std::string_view bright_white_foreground_seq = "\033[97m";
  inline constexpr std::string_view green_background_seq = "\033[42m";
  inline constexpr std::string_view yellow_background_seq = "\033[43m";
  inline constexpr std::string_view blue_background_seq = "\033[44m";
  inline constexpr std::string_view magenta_background_seq = "\033[45m";
  inline constexpr std::string_view cyan_background_seq = "\033[46m";
  inline constexpr std::string_view bright_blue_background_seq = "\033[104m";
  inline constexpr std::string_view bright_magenta_background_seq = "\033[105m";
  inline constexpr std::string_view bright_cyan_background_seq = "\033[106m";

  /**
   * @brief Returns an IO manipulator that outputs the given sequence if color is enabled
   */
  template <typename T>
  [[nodiscard]] auto manip(T& ctx, std::string_view sequence) noexcept
  {
    using manip_type = typename detail::context_traits<T>::manip_type;

    return manip_type{ctx, sequence};
  }

  /**
   * @brief Returns an IO manipulator that changes the foreground color to red if color is enabled
   */
  template <context T>
  [[nodiscard]] auto red(T& ctx) noexcept
  {
    return manip(ctx, red_foreground_seq);
  }

  /**
   * @brief Returns an IO manipulator that changes the foreground color to white if color is enabled
   */
  template <context T>
  [[nodiscard]] auto white(T& ctx) noexcept
  {
    return manip(ctx, white_foreground_seq);
  }

  /**
   * @brief Returns an IO manipulator that changes the foreground color to bright red if color is enabled
   */
  template <context T>
  [[nodiscard]] auto bright_red(T& ctx) noexcept
  {
    return manip(ctx, bright_red_foreground_seq);
  }

  /**
   * @brief Returns an IO manipulator that changes the foreground color to bright white if color is enabled
   */
  template <context T>
  [[nodiscard]] auto bright_white(T& ctx) noexcept
  {
    return manip(ctx, bright_white_foreground_seq);
  }

  /**
   * @brief Returns an IO manipulator that resets color if the color has been changed in the given context
   */
  template <context T>
  [[nodiscard]] auto reset(T& ctx) noexcept
  {
    using manip_type = typename detail::context_traits<T>::manip_reset_type;

    return manip_type{ctx};
  }

} // namespace yarisc::utils::color

#endif
