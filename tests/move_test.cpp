/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the MOV instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a register `r3` to register `r2` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r2, r3)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r2, r3");
      }
    }

    WHEN("register `r2` has value `0xfefe`, `r3` has value `0x1234`, and the status flags set")
    {
      current.set_r2(0xfefe);
      current.set_r3(0x1234);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0x1234);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0x1234` and the zero flag shall be cleared")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a short immediate `0x0` to register `r2` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r2, short_immediate{0x0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r2, 0");
      }
    }

    WHEN("register `r2` has value `0xfefe`")
    {
      current.set_r2(0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0x0);
        expected.set_status(yarisc::test::status_z);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0x0` and the zero flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a short immediate `0x5` to register `r3` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r3, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r3, 5");
      }
    }

    WHEN("register `r3` has value `0xfefe`")
    {
      current.set_r3(0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x5);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x5`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a short immediate `0xf` to register `r4` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r4, short_immediate{0xf})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r4, 0xf");
      }
    }

    WHEN("register `r4` has value `0xfefe`")
    {
      current.set_r4(0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0xf);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0xf`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an immediate `0xff` to register `r5` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r5, immediate), 0xff};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r5, 0xff");
      }
    }

    WHEN("register `r5` has value `0xfefe`")
    {
      current.set_r5(0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xff);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xff`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an immediate `0xabcd` to register `r0` MOV instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::move>(r0, immediate), 0xabcd};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "MOV r0, 0xabcd");
      }
    }

    WHEN("register `r0` has value `0xfefe`")
    {
      current.set_r0(0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xabcd);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xabcd`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
