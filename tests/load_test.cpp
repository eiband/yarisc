/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the LDR instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an LDR instruction that loads from an address in register `r1` into `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load>(r2, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDR r2, [r1]");
      }
    }

    WHEN("register `r1` has value `0x0010`, `r2` has value `0xfefe`, and memory at `0x0010` is `0xabcd`")
    {
      current.set_r1(0x0010);
      current.set_r2(0xfefe);
      current.store(0x0010, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0xabcd`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an LDR instruction that loads from the short immediate address `0x2` into `r3`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load>(r3, short_immediate{0x2})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDR r3, [2]");
      }
    }

    WHEN("`r3` has value `0xfefe`, memory at `0x0002` is `0xabcd`, and the status flags set")
    {
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_zc);
      current.store(0x0002, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0xabcd` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an LDR instruction that loads from the short immediate address `0xfffe` into `r3`")
  {
    yarisc::test::machine current{
      yarisc::test::max_memory, yarisc::arch::assemble<opcode::load>(r3, short_immediate{0xfffe})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDR r3, [0xfffe]");
      }
    }

    WHEN("`r3` has value `0xfefe`, memory at `0xfffe` is `0xabcd`, and the status flags set")
    {
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_zc);
      current.store(0xfffe, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0xabcd` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r3` has value `0xfefe`, memory at `0xfffe` is `0x0`")
    {
      current.set_r3(0xfefe);
      current.store(0xfffe, 0x0);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0` and the zero flag shall not be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an LDR instruction that loads from the immediate address `0x0020` into `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load>(r4, immediate), 0x0020};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDR r4, [0x20]");
      }
    }

    WHEN("`r4` has value `0xfefe` and memory at `0x0020` is `0x1234`")
    {
      current.set_r4(0xfefe);
      current.store(0x0020, 0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x1234);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x1234`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the LDX instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an LDX instruction that loads from an address in register `r1` with offset `r2` into `r3`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load_indexed>(r3, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDX r3, [r1, r2]");
      }
    }

    WHEN("registers `r1`, `r2`, `r3` have values `0x0012`, `0xfffe`, `0xfefe`, and memory at `0x0010` is `0xabcd`")
    {
      current.set_r1(0x0012);
      current.set_r2(0xfffe);
      current.set_r3(0xfefe);
      current.store(0x0010, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0xabcd`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an LDX instruction that loads from the short immediate offset `0x2` relative to `r5` into `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load_indexed>(r4, short_immediate{0x2}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDX r4, [r5, 2]");
      }
    }

    WHEN("`r4` has value `0xfefe`, `r5` has value `0x0004`, memory at `0x0006` is `0xabcd`, and the status flags set")
    {
      current.set_r4(0xfefe);
      current.set_r5(0x0004);
      current.set_status(yarisc::test::status_zc);
      current.store(0x0006, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0xabcd` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an LDX instruction that loads from the short immediate offset `0xfffe` relative to `sp` into `r3`")
  {
    yarisc::test::machine current{
      yarisc::test::max_memory, yarisc::arch::assemble<opcode::load_indexed>(r3, accumulator, short_immediate{0xfffe})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDX r3, [sp, 0xfffe]");
      }
    }

    WHEN("`r3` has value `0xfefe`, `sp` has value `0x0008`, memory at `0x0006` is `0xabcd`, and the status flags set")
    {
      current.set_r3(0xfefe);
      current.set_sp(0x0008);
      current.set_status(yarisc::test::status_zc);
      current.store(0x0006, 0xabcd);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0xabcd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0xabcd` and the status flags shall be unchanged")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r3` has value `0xfefe`, `sp` has value `0x0008`, memory at `0x0006` is `0x0`")
    {
      current.set_r3(0xfefe);
      current.set_sp(0x0008);
      current.store(0x0006, 0x0);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0` and the zero flag shall not be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN(
    "a test machine with an LDX instruction that loads from the immediate offset `0x0020` relative to `r3` into `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::load_indexed>(r4, r3,immediate), 0x0020};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "LDX r4, [r3, 0x20]");
      }
    }

    WHEN("`r3` has value `0x0008`, `r4` has value `0xfefe` and memory at `0x0028` is `0x1234`")
    {
      current.set_r3(0x0008);
      current.set_r4(0xfefe);
      current.store(0x0028, 0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x1234);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x1234`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
