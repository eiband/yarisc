/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <emu/emulator.hpp>

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
  using namespace yarisc::arch;
  using namespace yarisc::emu;

  try
  {
    emulator em{emulator::default_level, emulator_mode::interactive};

    if (!em.execute(execution_mode::strict))
    {
      std::cerr << "A breakpoint was hit" << std::endl;

      return 1;
    }
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Exception: " << ex.what() << std::endl;

    return 1;
  }

  return 0;
}
