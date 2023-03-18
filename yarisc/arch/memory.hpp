/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_ARCH_MEMORY_HPP
#define YARISC_ARCH_MEMORY_HPP

#include <yarisc/arch/detail/endianness.hpp>
#include <yarisc/arch/export.h>
#include <yarisc/arch/output.hpp>
#include <yarisc/arch/types.hpp>

#include <cassert>
#include <cstddef>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <utility>

namespace yarisc::arch
{
  struct machine_state;

  namespace detail
  {
    [[nodiscard]] inline constexpr bool is_aligned(std::size_t address) noexcept
    {
      static_assert(sizeof(word_t) == 2);

      return ((address & 0x1) == 0x0);
    }

    inline void throw_if_not_aligned(std::size_t address)
    {
      if (!is_aligned(address))
        throw std::invalid_argument{"unaligned memory view"};
    }

    inline void throw_if_out_of_range(std::size_t off, std::size_t size)
    {
      if (off > size)
        throw std::out_of_range{"memory out of range"};
    }

  } // namespace detail

  /**
   * @brief Non-owning aligned view of a machine memeory
   *
   * A memory stores a word-aligned pointer into the memory of the machine. In addition the base address and an optional
   * reference to the machine state is stored for visualization.
   *
   * The sizes and offsets are in bytes. However, exceptions are thrown if they are not word-aligned.
   */
  class memory_view final
  {
  public:
    using value_type = std::byte;
    using size_type = std::size_t;

    using const_iterator = const value_type*;
    using iterator = const_iterator;

    static constexpr size_type npos = static_cast<size_type>(-1);

    /**
     * @brief Constructor
     *
     * Constructs an empty view.
     */
    memory_view() noexcept = default;

    /**
     * @brief Constructor
     *
     * Throws an invalid argument exception if the size or the base address are not a word-aligned.
     *
     * @note
     * The base address is used for visualization. Memory before the start address and beyond the view is not guaranteed
     * to be dereferenceable even though the base address could suggest otherwise.
     *
     * @param d pointer to the start address
     * @param sz size of the memory area in bytes
     * @param bs base address of the memory area
     * @param st optional pointer to the machine state for visualization
     */
    memory_view(const value_type* d, size_type sz, address_t bs = 0, const machine_state* st = nullptr)
      : data_{d}
      , size_{sz}
      , base_{bs}
      , state_{st}
    {
      detail::throw_if_not_aligned(sz);
      detail::throw_if_not_aligned(bs);
    }

    /**
     * @brief Returns the begin iterator of bytes
     */
    [[nodiscard]] const_iterator begin() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns the begin iterator of bytes
     */
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns the end iterator of bytes
     */
    [[nodiscard]] const_iterator end() const noexcept
    {
      return data_ + size_;
    }

    /**
     * @brief Returns the end iterator of bytes
     */
    [[nodiscard]] const_iterator cend() const noexcept
    {
      return data_ + size_;
    }

    /**
     * @brief Returns the pointer to the start of the memory view
     */
    [[nodiscard]] const value_type* data() const noexcept
    {
      return data_;
    }

    /**
     * @brief Returns the size of the memory in bytes
     */
    [[nodiscard]] size_type size() const noexcept
    {
      return size_;
    }

    /**
     * @brief Returns the byte address in machine memory that corresponds to the start address
     */
    [[nodiscard]] address_t base() const noexcept
    {
      return base_;
    }

    /**
     * @brief Returns a pointer to the machine state or nullptr
     */
    [[nodiscard]] const machine_state* state() const noexcept
    {
      return state_;
    }

    /**
     * @brief Returns if the memory area is empty
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return (size_ == 0);
    }

    /**
     * @brief Returns the byte at the given offset
     *
     * Behavior is undefined unless `off` is less than the size of the memory view.
     *
     * @param off offset into the memory view in bytes
     * @return byte at the given offset
     */
    [[nodiscard]] value_type operator[](size_type off) const noexcept
    {
      assert(off < size_);

      return data_[off];
    }

    /**
     * @brief Returns a view into a subarea of the memory
     *
     * The given size `sz` is trimmed to the end of this memory view. Throws an invalid argument exception if `off` or
     * the trimmed size are not word-aligned. An out-of-range exception is thrown if `off` is outside of the view.
     *
     * @param off offset into the memory view in bytes
     * @param sz maximum size in bytes of the returned view
     * @return memory view of a sub area
     */
    [[nodiscard]] memory_view sub(size_type off, size_type sz = npos) const
    {
      detail::throw_if_not_aligned(off);
      detail::throw_if_out_of_range(off, size_);

      address_t bs = base_;

      if (const size_type max_sz = size_ - off; sz > max_sz)
        sz = max_sz;

      bs += static_cast<address_t>(off);

      return {data_ + off, sz, bs, state_};
    }

  private:
    const value_type* data_{nullptr};

    size_type size_{0};
    address_t base_{0};

    const machine_state* state_{nullptr};
  };

  /**
   * @brief Returns the begin iterator of bytes
   */
  [[nodiscard]] inline memory_view::const_iterator begin(const memory_view& view) noexcept
  {
    return view.begin();
  }

  /**
   * @brief Returns the begin iterator of bytes
   */
  [[nodiscard]] inline memory_view::const_iterator cbegin(const memory_view& view) noexcept
  {
    return view.cbegin();
  }

  /**
   * @brief Returns the end iterator of bytes
   */
  [[nodiscard]] inline memory_view::const_iterator end(const memory_view& view) noexcept
  {
    return view.end();
  }

  /**
   * @brief Returns the end iterator of bytes
   */
  [[nodiscard]] inline memory_view::const_iterator cend(const memory_view& view) noexcept
  {
    return view.cend();
  }

  YARISC_ARCH_EXPORT void tag_invoke(output_t, std::ostream& os, memory_view mem, output_format fmt);
  YARISC_ARCH_EXPORT void tag_invoke(output_t, std::ostream& os, diff<memory_view> mem, output_format fmt);

  /**
   * @brief Main memory of the machine
   *
   * This class owns a contiguous block of the entire word-aligned memory of the machine.
   *
   * The sizes and offsets are in bytes. However, exceptions are thrown if they are not word-aligned.
   */
  class memory final
  {
  public:
    using value_type = std::byte;
    using size_type = std::size_t;

    using iterator = value_type*;
    using const_iterator = const value_type*;

    static constexpr size_type npos = static_cast<size_type>(-1);

    /**
     * @brief Constructor
     *
     * Allocates the maximum size that can be addressed by the machine.
     */
    YARISC_ARCH_EXPORT memory();

    /**
     * @brief Constructor
     *
     * Throws an invalid argument exception if the size is not word-aligned.
     *
     * @param sz number of bytes to allocate
     */
    YARISC_ARCH_EXPORT explicit memory(size_type sz);

    /**
     * @brief Copy constructor
     *
     * @param that memory to copy
     */
    YARISC_ARCH_EXPORT memory(const memory& that);

    /**
     * @brief Move constructor
     *
     * @param that memory to move
     */
    memory(memory&& that) noexcept
      : data_{std::move(that.data_)}
      , size_{std::exchange(that.size_, 0)}
    {
    }

    /**
     * @brief Copy assignment
     *
     * @param that memory to copy
     * @return reference to this
     */
    memory& operator=(const memory& that)
    {
      memory{that}.swap(*this);

      return *this;
    }

    /**
     * @brief Move assignment
     *
     * @param that memory to move
     * @return reference to this
     */
    memory& operator=(memory&& that) noexcept
    {
      memory{std::move(that)}.swap(*this);

      return *this;
    }

    /**
     * @brief Returns the begin iterator of bytes
     */
    [[nodiscard]] iterator begin() noexcept
    {
      return data_.get();
    }

    /**
     * @brief Returns the begin iterator of bytes
     */
    [[nodiscard]] const_iterator begin() const noexcept
    {
      return data_.get();
    }

    /**
     * @brief Returns the begin iterator of bytes
     */
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
      return data_.get();
    }

    /**
     * @brief Returns the end iterator of bytes
     */
    [[nodiscard]] iterator end() noexcept
    {
      return data_.get() + size_;
    }

    /**
     * @brief Returns the end iterator of bytes
     */
    [[nodiscard]] const_iterator end() const noexcept
    {
      return data_.get() + size_;
    }

    /**
     * @brief Returns the end iterator of bytes
     */
    [[nodiscard]] const_iterator cend() const noexcept
    {
      return data_.get() + size_;
    }

    /**
     * @brief Returns the pointer to the first byte in memory
     */
    [[nodiscard]] value_type* data() noexcept
    {
      return data_.get();
    }

    /**
     * @brief Returns the pointer to the first byte in memory
     */
    [[nodiscard]] const value_type* data() const noexcept
    {
      return data_.get();
    }

    /**
     * @brief Returns the size of the memory in bytes
     */
    [[nodiscard]] size_type size() const noexcept
    {
      return size_;
    }

    /**
     * @brief Returns if the memory area is empty
     */
    [[nodiscard]] bool empty() const noexcept
    {
      return (size_ == 0);
    }

    /**
     * @brief Clears the memory to all zeros
     */
    YARISC_ARCH_EXPORT void clear() noexcept;

    /**
     * @brief Loads a word from memory
     *
     * Behavior is undefined unless the address is word-aligned.
     *
     * @param address byte address into the memory
     * @return memory word at the given address
     */
    [[nodiscard]] word_t load(address_t address) const noexcept
    {
      assert(address < size_);
      assert(detail::is_aligned(address));

      return detail::load_word(data_.get() + address);
    }

    /**
     * @brief Stores a word to memory
     *
     * Behavior is undefined unless the address is word-aligned.
     *
     * @param address byte address into the memory
     * @param value memory word to store
     */
    void store(address_t address, word_t value) noexcept
    {
      assert(address < size_);
      assert(detail::is_aligned(address));

      return detail::store_word(data_.get() + address, value);
    }

    /**
     * @brief Returns a reference to the byte at the given address
     *
     * Behavior is undefined unless `off` is less than the size of the memory.
     *
     * @param off byte address into the memory
     * @return reference to the byte at the given address
     */
    [[nodiscard]] value_type& operator[](size_type off) noexcept
    {
      assert(off < size_);

      return data_[off];
    }

    /**
     * @brief Returns the byte at the given offset
     *
     * Behavior is undefined unless `off` is less than the size of the memory.
     *
     * @param off byte address into the memory
     * @return byte at the given address
     */
    [[nodiscard]] value_type operator[](size_type off) const noexcept
    {
      assert(off < size_);

      return data_[off];
    }

    /**
     * @brief Returns a view into a subarea of the memory
     *
     * The given size `sz` is trimmed to the end of memory. Throws an invalid argument exception if `off` or the trimmed
     * size are not word-aligned. An out-of-range exception is thrown if `off` is outside of the memory area.
     *
     * @param off byte address into the memory
     * @param sz maximum size in bytes of the returned view
     * @param st optional pointer to the machine state for visualization
     * @return memory view of a sub area
     */
    [[nodiscard]] memory_view sub(size_type off, size_type sz = npos, const machine_state* st = nullptr) const
    {
      detail::throw_if_not_aligned(off);
      detail::throw_if_out_of_range(off, size_);

      if (const size_type max_sz = size_ - off; sz > max_sz)
        sz = max_sz;

      return {data_.get() + off, sz, static_cast<address_t>(off), st};
    }

    /**
     * @brief Returns a view of the whole memory area
     *
     * @param st optional pointer to the machine state for visualization
     * @return memory view of whole memory
     */
    [[nodiscard]] memory_view view(const machine_state* st = nullptr) const noexcept
    {
      return {data_.get(), size_, 0, st};
    }

    /**
     * @brief Returns a view of the whole memory area
     */
    [[nodiscard]] operator memory_view() const noexcept
    {
      return {data_.get(), size_};
    }

    /**
     * @brief Comparison operator
     *
     * @param that memory to compare with
     * @return true if equal, false otherwise
     */
    [[nodiscard]] YARISC_ARCH_EXPORT bool operator==(const memory& that) const noexcept;

    /**
     * @brief Swaps with another memory instances
     *
     * @param that instance to swap with
     */
    void swap(memory& that) noexcept
    {
      using std::swap;

      swap(data_, that.data_);
      swap(size_, that.size_);
    }

  private:
    std::unique_ptr<value_type[]> data_{};

    size_type size_{0};
  };

  /**
   * @brief Returns the begin iterator of bytes
   */
  [[nodiscard]] inline memory::iterator begin(memory& view) noexcept
  {
    return view.begin();
  }

  /**
   * @brief Returns the begin iterator of bytes
   */
  [[nodiscard]] inline memory::const_iterator begin(const memory& view) noexcept
  {
    return view.begin();
  }

  /**
   * @brief Returns the begin iterator of bytes
   */
  [[nodiscard]] inline memory::const_iterator cbegin(const memory& view) noexcept
  {
    return view.cbegin();
  }

  /**
   * @brief Returns the end iterator of bytes
   */
  [[nodiscard]] inline memory::iterator end(memory& view) noexcept
  {
    return view.end();
  }

  /**
   * @brief Returns the end iterator of bytes
   */
  [[nodiscard]] inline memory::const_iterator end(const memory& view) noexcept
  {
    return view.end();
  }

  /**
   * @brief Returns the end iterator of bytes
   */
  [[nodiscard]] inline memory::const_iterator cend(const memory& view) noexcept
  {
    return view.cend();
  }

  /**
   * @brief Swaps two memory instances
   *
   * @param lhs first instance
   * @param rhs second instance
   */
  inline void swap(memory& lhs, memory& rhs) noexcept
  {
    lhs.swap(rhs);
  }

} // namespace yarisc::arch

#endif
