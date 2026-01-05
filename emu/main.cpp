/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <emu/emulator.hpp>

#include <exception>
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[])
{
  using namespace yarisc::arch;
  using namespace yarisc::emu;

  try
  {
    // Load image from command line. An empty string means that there is no image.
    const std::filesystem::path image{(argc == 2) ? argv[1] : ""};

    emulator em{image, emulator::default_level, emulator_mode::interactive};

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
