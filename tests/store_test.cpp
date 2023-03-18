/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the STR instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a STR instruction that stores the value of `r2` to an address in register `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store>(r2, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STR r2, r1");
      }
    }

    WHEN("register `r1` has value `0x0010`, `r2` has value `0xabcd`, and memory at `0x0010` is `0xfefe`")
    {
      current.set_r1(0x0010);
      current.set_r2(0xabcd);
      current.store(0x0010, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0010, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0x0010`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a STR instruction that stores the value of `r3` to the short immediate address `0xe`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store>(r3, short_immediate{0xe})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STR r3, 0xe");
      }
    }

    WHEN("register `r3` has value `0xabcd`, memory at `0x000e` is `0xfefe`, and the status flags set")
    {
      current.set_r3(0xabcd);
      current.set_status(yarisc::test::status_zc);
      current.store(0x000e, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x000e, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0x000e` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a STR instruction that stores the value of `r3` to the immediate address `0x0020`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store>(r3, immediate), 0x0020};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STR r3, 0x20");
      }
    }

    WHEN("register `r3` has value `0x1234` and memory at `0x0020` is `0xfefe`")
    {
      current.set_r3(0x1234);
      current.store(0x0020, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0020, 0x1234);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the value `0x1234` shall have been written at address `0x0020`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
