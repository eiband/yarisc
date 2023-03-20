/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/arch/memory.hpp>

#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/detail/hex_memory.hpp>

#include <cstring>
#include <limits>
#include <type_traits>

namespace yarisc::arch
{
  namespace
  {
    static_assert(std::is_unsigned_v<address_t>);
    static_assert(std::is_unsigned_v<memory::size_type>);

    static_assert(sizeof(address_t) < sizeof(memory::size_type));

    constexpr auto max_size  = static_cast<memory::size_type>(std::numeric_limits<address_t>::max()) + 1;

    static_assert(detail::is_aligned(max_size));

    template <typename T>
    void format_memory_view(std::ostream& os, const T& mem, output_format fmt)
    {
      detail::format_output(detail::hex_memory, os, fmt, mem);
    }

  } // namespace

  void tag_invoke(output_t, std::ostream& os, memory_view mem, output_format fmt)
  {
    format_memory_view(os, mem, fmt);
  }

  void tag_invoke(output_t, std::ostream& os, diff<memory_view> mem, output_format fmt)
  {
    format_memory_view(os, mem, fmt);
  }

  memory::memory()
    : data_{std::make_unique<std::byte[]>(max_size)}
    , size_{max_size}
  {
    std::memset(data_.get(), 0, max_size);
  }

  memory::memory(const memory& that)
    : size_{that.size_}
  {
    if (size_ > 0)
    {
      assert(that.data_);

      data_ = std::make_unique<std::byte[]>(size_);

      std::memcpy(data_.get(), that.data_.get(), size_);
    }
  }

  memory::memory(size_type sz)
    : size_{sz}
  {
    detail::throw_if_not_aligned(sz);

    if (size_ > 0)
    {
      if (size_ > max_size)
        throw std::out_of_range{"memory not addressable"};

      data_ = std::make_unique<std::byte[]>(size_);

      std::memset(data_.get(), 0, size_);
    }
  }

  void memory::clear() noexcept
  {
    if (data_)
      std::memset(data_.get(), 0, size_);
  }

  bool memory::operator==(const memory& that) const noexcept
  {
    if (size_ != that.size_)
      return false;

    return (size_ == 0) || (std::memcmp(data_.get(), that.data_.get(), size_) == 0);
  }

} // namespace yarisc::arch
