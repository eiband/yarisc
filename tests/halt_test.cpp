/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the HLT instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a HLT instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::halt>()};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "HLT");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip();

      const bool keep_going = current.execute_instruction();

      THEN("the machine shall be halted")
      {
        REQUIRE(!keep_going);
        CHECK(current == expected);
      }
    }
  }
}
