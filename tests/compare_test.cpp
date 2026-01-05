/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the CMP instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an CMP instruction using registers `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP r1, r2");
      }
    }

    WHEN("register `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r1(0x106c);
      current.set_r2(0x094b);
      current.set_status(yarisc::test::status_nzcv);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0x0001`, and `r2` has value `0x0001`")
    {
      current.set_r1(0x0001);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the zero and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0x0001`, and `r2` has value `0x0002`, and the carry flags set")
    {
      current.set_r1(0x0001);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0x0000`, and `r2` has value `0x0001`")
    {
      current.set_r1(0x0000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0xf61e`, and `r2` has value `0xf5a4`")
    {
      current.set_r1(0xf61e);
      current.set_r2(0xf5a4);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0x4000`, and `r2` has value `0xc000`")
    {
      current.set_r1(0x4000);
      current.set_r2(0xc000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_nv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the negative and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("`r1` has value `0x8000`, and `r2` has value `0x0001`")
    {
      current.set_r1(0x8000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the zero and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(short_immediate{0x6}, r5)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(short_immediate{0xfff9}, r5)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(r4, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0x8002);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("only the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using a left-hand immediate `0xf555` with register `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP 0xf555, r4");
      }
    }

    WHEN("register `r4` has value `0x0d00`")
    {
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an CMP instruction using a right-hand immediate `0x0203` with register `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::compare>(r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "CMP r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050` and the zero flag set")
    {
      current.set_r0(0x1050);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
