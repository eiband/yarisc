/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the NOP instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a NOP instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::noop>()};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "NOP");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip();

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall be incremented")
      {
        CHECK(current == expected);
      }
    }
  }
}
