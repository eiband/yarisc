/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_DETAIL_FUNCTIONAL_HPP
#define YARISC_ARCH_DETAIL_FUNCTIONAL_HPP

#include <type_traits>
#include <utility>

namespace yarisc::arch::detail
{
  namespace _custom
  {
    void tag_invoke();

    // clang-format off

    template <typename Tag, typename... Args>
    concept tag_invocable =
      requires(Tag&& tag, Args&&... args) {
        tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
      };

    template <typename Tag, typename... Args>
    concept nothrow_tag_invocable =
      tag_invocable<Tag, Args...>
      && requires(Tag&& tag, Args&&... args) {
           {  tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...) } noexcept;
         };

    // clang-format on

    template <typename Tag, typename... Args>
    using tag_invoke_result_t = decltype(tag_invoke(declval<Tag>(), declval<Args>()...));

    template <typename Tag, typename... Args>
    struct tag_invoke_result
    {
    };

    template <typename Tag, typename... Args>
      requires tag_invocable<Tag, Args...>
    struct tag_invoke_result<Tag, Args...>
    {
      using type = tag_invoke_result_t<Tag, Args...>;
    };

    struct fn
    {
      template <typename Tag, typename... Args>
      constexpr auto operator()(Tag&& tag, Args&&... args) const
        noexcept(noexcept(tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...)))
          -> decltype(tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...))
      {
        return tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
      }
    };

  } // namespace _custom

  namespace custom
  {
    using _custom::tag_invocable;

    using _custom::nothrow_tag_invocable;

    using _custom::tag_invoke_result;
    using _custom::tag_invoke_result_t;

    inline constexpr _custom::fn tag_invoke{};

    template <auto& Tag>
    using tag_t = std::decay_t<decltype(Tag)>;

  } // namespace custom

} // namespace yarisc::arch::detail

#endif
