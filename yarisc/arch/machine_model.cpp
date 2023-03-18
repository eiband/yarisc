/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <yarisc/arch/machine_model.hpp>

#include <yarisc/arch/detail/format.hpp>
#include <yarisc/arch/detail/hex_registers.hpp>

namespace yarisc::arch
{
  namespace
  {
    template <typename T>
    void format_registers(std::ostream& os, const T& reg, output_format fmt)
    {
      detail::format_output(detail::hex_registers, os, fmt, reg);
    }

  } // namespace

  void tag_invoke(output_t, std::ostream& os, const registers_view& reg, output_format fmt)
  {
    format_registers(os, reg, fmt);
  }

  void tag_invoke(output_t, std::ostream& os, const diff<registers_view>& reg, output_format fmt)
  {
    format_registers(os, reg, fmt);
  }

} // namespace yarisc::arch
