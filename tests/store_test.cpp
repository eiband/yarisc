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
        CHECK(text == "STR r2, [r1]");
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

  GIVEN("a test machine with a STR instruction that stores the value of `r3` to the short immediate address `0x6`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store>(r3, short_immediate{0x6})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STR r3, [6]");
      }
    }

    WHEN("register `r3` has value `0xabcd`, memory at `0x0006` is `0xfefe`, and the status flags set")
    {
      current.set_r3(0xabcd);
      current.set_status(yarisc::test::status_nzcv);
      current.store(0x0006, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0006, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0x0006` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a STR instruction that stores the value of `r3` to the short immediate address `0xfff8`")
  {
    yarisc::test::machine current{
      yarisc::test::max_memory, yarisc::arch::assemble<opcode::store>(r3, short_immediate{0xfff8})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STR r3, [0xfff8]");
      }
    }

    WHEN("register `r3` has value `0xabcd`, memory at `0xfff8` is `0xfefe`, and the status flags set")
    {
      current.set_r3(0xabcd);
      current.set_status(yarisc::test::status_nzcv);
      current.store(0xfff8, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0xfff8, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0xfff8` and the status flags shall be unchanged")
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
        CHECK(text == "STR r3, [0x20]");
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

SCENARIO("execute the STX instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a STX instruction that stores the value of `r1` to an address in register `r2` with "
        "offset `r3`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store_indexed>(r1, r2, r3)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STX r1, [r2, r3]");
      }
    }

    WHEN("registers `r1`, `r2`, `r3` have values `0xabcd`, `0x0006`, `0x000a`, and memory at `0x0010` is `0xfefe`")
    {
      current.set_r1(0xabcd);
      current.set_r2(0x0006);
      current.set_r3(0x000a);
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

  GIVEN("a test machine with a STX instruction that stores the value of `r3` to the short immediate offset `0x6` "
        "relative to `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store_indexed>(r3, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STX r3, [r5, 6]");
      }
    }

    WHEN("register `r3` has value `0xabcd`, `r5` has value `0x0002`, memory at `0x0008` is `0xfefe`, and the "
         "status flags set")
    {
      current.set_r3(0xabcd);
      current.set_r5(0x0002);
      current.set_status(yarisc::test::status_nzcv);
      current.store(0x0008, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0008, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0x0008` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a STX instruction that stores the value of `r3` to the short immediate offset `0xfff8` "
        "relative to `sp`")
  {
    yarisc::test::machine current{
      yarisc::test::max_memory,
      yarisc::arch::assemble<opcode::store_indexed>(r3, accumulator, short_immediate{0xfff8})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STX r3, [sp, 0xfff8]");
      }
    }

    WHEN("register `r3` has value `0xabcd`, `sp` has value `0x000a`, memory at `0x0002` is `0xfefe`, and the status "
         "flags set")
    {
      current.set_r3(0xabcd);
      current.set_sp(0x000a);
      current.set_status(yarisc::test::status_nzcv);
      current.store(0x0002, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0002, 0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the value `0xabcd` shall have been written at address `0x0002` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a STX instruction that stores the value of `r3` to the immediate offset `0x0020` relative "
        "to `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::store_indexed>(r3, r4, immediate), 0x0020};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "STX r3, [r4, 0x20]");
      }
    }

    WHEN("register `r3` has value `0x1234`, `r4` has value `0x0004`, and memory at `0x0024` is `0xfefe`")
    {
      current.set_r3(0x1234);
      current.set_r4(0x0004);
      current.store(0x0024, 0xfefe);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.store(0x0024, 0x1234);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the value `0x1234` shall have been written at address `0x0024`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
