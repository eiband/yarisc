/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the SUB instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an SUB instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x106c);
      current.set_r2(0x094b);
      current.set_status(yarisc::test::status_nzcv);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0721);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0721`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN(
      "register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0002`, and the carry flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xf61e`, and `r2` has value `0xf5a4`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xf61e);
      current.set_r2(0xf5a4);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x007a);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x007a`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x4000`, and `r2` has value `0xc000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x4000);
      current.set_r2(0xc000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x8000);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x8000`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x8000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x8000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x7fff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x7fff`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0x0000);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0x0000`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xf005);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xf005`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r5, short_immediate{0xfff9}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r5, 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xeff8);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xeff8`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0x8002);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x7ffd);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x7ffd`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0xe855);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0xe855`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUB instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUB r3, r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050`, `r3` has value `0xfefe`, and the zero flag set")
    {
      current.set_r0(0x1050);
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0e4d);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0e4d`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the SUBS instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an SUBS instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x106c);
      current.set_r2(0x094b);
      current.set_status(yarisc::test::status_nzcv);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0721);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0721` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000` and the zero and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN(
      "register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0002`, and the carry flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xf61e`, and `r2` has value `0xf5a4`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xf61e);
      current.set_r2(0xf5a4);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x007a);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x007a` and the carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x4000`, and `r2` has value `0xc000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x4000);
      current.set_r2(0xc000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x8000);
        expected.set_status(yarisc::test::status_nv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x8000` and the negative and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x8000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x8000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x7fff);
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x7fff` and the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0x0000);
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0x0000` and the zero and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xf005);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xf005` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r5, short_immediate{0xfff9}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r5, 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xeff8);
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xeff8` and the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0x8002);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x7ffd);
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x7ffd` and only the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0xe855);
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0xe855` and the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SUBS instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SUBS r3, r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050`, `r3` has value `0xfefe`, and the zero flag set")
    {
      current.set_r0(0x1050);
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0e4d);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0e4d` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the SBC instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an SBC instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub_with_borrow>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x106c);
      current.set_r2(0x094b);
      current.set_status(yarisc::test::status_nzcv);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0721);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0721`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN(
      "register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0002`, and the carry flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xfffe);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xfffe`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xf61e`, and `r2` has value `0xf5a4`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xf61e);
      current.set_r2(0xf5a4);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0079);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0079`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x4000`, and `r2` has value `0xbfff`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x4000);
      current.set_r2(0xbfff);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x8000);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x8000`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x8000`, and `r2` has value `0x0000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x8000);
      current.set_r2(0x0000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x7fff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x7fff`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub_with_borrow>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0xffff`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::sub_with_borrow>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xf004);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xf004`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::sub_with_borrow>(r5, short_immediate{0xfff9}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r5, 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xeff7);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xeff7`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::sub_with_borrow>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0x8002);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x7ffc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x7ffc`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub_with_borrow>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0xe854);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0xe854`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBC instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::sub_with_borrow>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBC r3, r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050`, `r3` has value `0xfefe`, and the zero flag set")
    {
      current.set_r0(0x1050);
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0e4c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0e4c`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the SBCS instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an SBCS instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs_with_borrow>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x106c);
      current.set_r2(0x094b);
      current.set_status(yarisc::test::status_nzcv);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0721);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0721` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN(
      "register `r0` has value `0xfefe`, `r1` has value `0x0001`, and `r2` has value `0x0002`, and the carry flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0001);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xfffe);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xfffe` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xf61e`, and `r2` has value `0xf5a4`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xf61e);
      current.set_r2(0xf5a4);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0079);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0079` and the carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x4000`, and `r2` has value `0xbfff`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x4000);
      current.set_r2(0xbfff);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x8000);
        expected.set_status(yarisc::test::status_nv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x8000` and the negative and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x8000`, and `r2` has value `0x0000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x8000);
      current.set_r2(0x0000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x7fff);
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x7fff` and the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs_with_borrow>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0xffff);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0xffff` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::subs_with_borrow>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xf004);
        expected.set_status(yarisc::test::status_n);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xf004` and the negative flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::subs_with_borrow>(r5, short_immediate{0xfff9}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r5, 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0xeff7);
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0xeff7` and the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::subs_with_borrow>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0x8002);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x7ffc);
        expected.set_status(yarisc::test::status_cv);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x7ffc` and only the carry and overflow flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs_with_borrow>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0xe854);
        expected.set_status(yarisc::test::status_nc);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0xe854` and the negative and carry flags shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an SBCS instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::subs_with_borrow>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "SBCS r3, r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050`, `r3` has value `0xfefe`, and the zero flag set")
    {
      current.set_r0(0x1050);
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x0e4c);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x0e4c` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
