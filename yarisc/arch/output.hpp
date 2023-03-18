/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_OUTPUT_HPP
#define YARISC_ARCH_OUTPUT_HPP

#include <yarisc/arch/detail/functional.hpp>

#include <iosfwd>
#include <utility>

namespace yarisc::arch
{
  /**
   * @brief Colored text output format
   */
  enum class output_format
  {
    /**
     * @brief Checks if the console supports color
     */
    console,

    /**
     * @brief Disable color output
     */
    plain,

    /**
     * @brief Enable color output
     */
    colored,
  };

  namespace detail::_output
  {
    namespace cpo
    {
      struct fn
      {
        explicit fn() = default;

        template <typename T>
          requires custom::tag_invocable<fn, std::ostream&, const T&, output_format>
        std::ostream& operator()(std::ostream& os, const T& obj, output_format fmt = output_format::console) const
        {
          custom::tag_invoke(*this, os, obj, fmt);

          return os;
        }
      };

    } // namespace cpo

  } // namespace detail::_output

  /**
   * @brief Type of the customization point object for outputting text to an output stream
   */
  using output_t = detail::_output::cpo::fn;

  /**
   * @brief Customization point object for outputting text to an output stream
   *
   * @code
   * void foo(std::ostream& os)
   * {
   *   registers reg;
   *
   *   // (1) uses default output format console
   *   output(os, reg);
   *
   *   // (2) it is also possible to override the output format
   *   output(os, reg, output_format::colored);
   *
   *   // (3) finally the following is equivalent to (1)
   *   os << reg;
   * }
   * @endcode
   */
  inline constexpr output_t output{};

  /**
   * @brief Concept that returns of if the customization point is implemented for the given type
   */
  template <typename T>
  concept outputable = requires(std::ostream& os, const T& obj) { output(os, obj); };

  /**
   * @brief Makes all types of this library that customize the output customization point streamable
   *
   * Uses the default output format console.
   *
   * @param os output stream
   * @param obj the object to be output
   * @return given output stream
   */
  template <outputable T>
  std::ostream& operator<<(std::ostream& os, const T& obj)
  {
    return output(os, obj);
  }

  /**
   * @brief Simple wrapper of a type to output a colored diff
   */
  template <typename T>
  struct diff
  {
    T current{};
    T previous{};
  };

  /**
   * @brief Returns a diff wrapper if it can be output
   */
  template <typename T>
    requires outputable<diff<T>>
  [[nodiscard]] auto as_diff(T current, T previous)
  {
    return diff<T>{std::move(current), std::move(previous)};
  }

} // namespace yarisc::arch

#endif
