/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#ifndef YARISC_UTILS_IOS_HPP
#define YARISC_UTILS_IOS_HPP

#include <ios>
#include <ostream>

namespace yarisc::utils
{
  /**
   * @brief Restores flags, width, and fill character of an output stream on destruction
   */
  class ostream_guard final
  {
  public:
    /**
     * @brief Constructor
     *
     * @param os output stream to restore on destruction
     */
    explicit ostream_guard(std::ostream& os)
      : ostream_{os}
    {
    }

    ostream_guard(const ostream_guard& that) = delete;
    ostream_guard(ostream_guard&& that) = delete;

    /**
     * @brief Destructor
     *
     * Restores flags, width, and fill character to the values at construction time of this guard.
     */
    ~ostream_guard()
    {
      ostream_.setf(flags_);
      ostream_.width(width_);
      ostream_.fill(fill_);
    }

    ostream_guard& operator=(const ostream_guard& that) = delete;
    ostream_guard& operator=(ostream_guard&& that) = delete;

  private:
    std::ostream& ostream_;

    std::ios_base::fmtflags flags_{ostream_.flags()};
    std::streamsize width_{ostream_.width()};
    char fill_{ostream_.fill()};
  };

} // namespace yarisc::utils

#endif
