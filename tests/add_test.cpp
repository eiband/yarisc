/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the ADD instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an ADD instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x094b);
      current.set_r2(0x106c);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x19b7);
        expected.clear_status();
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x19b7` and the zero and carry flags shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.set_status(yarisc::test::status_z);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000` and the zero flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xfffe`, and `r2` has value `0x0001`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xfffe);
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

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xfffe`, and `r2` has value `0x0002`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xfffe);
      current.set_r2(0x0002);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000` and the carry and zero flags shall be set")
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
        expected.set_r0(0xebc2);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xebc2` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234`")
    {
      current.set_r1(0x1234);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0x2468);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0x2468`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0x1007);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0x1007`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using a left-hand short immediate `0xfff9` with register `r5`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r5, short_immediate{0xfff9}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r5, 0xfff9, r5");
      }
    }

    WHEN("register `r5` has value `0x1001`")
    {
      current.set_r5(0x1001);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0x0ffa);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0x0ffa` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0xfffd);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x0002);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x0002` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0x0255);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0x0255` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADD instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADD r3, r0, 0x0203");
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
        expected.set_r3(0x1253);
        expected.clear_status();
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x1253` and the zero flag shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the ADC instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with an ADC instruction using registers `r0`, `r1`, `r2`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add_with_carry>(r0, r1, r2)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r0, r1, r2");
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x094b`, `r2` has value `0x106c`, and the status flags set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x094b);
      current.set_r2(0x106c);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x19b8);
        expected.clear_status();
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x19b8` and the zero and carry flags shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xffff`, `r2` has value `0xffff`, and the carry flag set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xffff);
      current.set_r2(0xffff);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0x0000`, and `r2` has value `0x0000`")
    {
      current.set_r0(0xfefe);
      current.set_r1(0x0000);
      current.set_r2(0x0000);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.set_status(yarisc::test::status_z);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000` and the zero flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xfffd`, `r2` has value `0x0001`, and the carry flag set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xfffd);
      current.set_r2(0x0001);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0xffff);
        expected.clear_status();
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xffff` and the carry flag shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("register `r0` has value `0xfefe`, `r1` has value `0xfffd`, `r2` has value `0x0002`, and the carry flag set")
    {
      current.set_r0(0xfefe);
      current.set_r1(0xfffd);
      current.set_r2(0x0002);
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r0(0x0000);
        expected.set_status(yarisc::test::status_zc);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0x0000` and the carry and zero flags shall be set")
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
        expected.set_r0(0xebc2);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r0` shall have the value `0xebc2` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADC instruction using only registers `r1`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add_with_carry>(r1, r1, r1)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r1, r1, r1");
      }
    }

    WHEN("register `r1` has value `0x1234` and the status flags set")
    {
      current.set_r1(0x1234);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r1(0x2469);
        expected.clear_status();
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r1` shall have the value `0x2469` and the zero and carry flags shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADC instruction using a left-hand short immediate `0x6` with register `r5`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::add_with_carry>(r5, short_immediate{0x6}, accumulator)};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r5, 6, r5");
      }
    }

    WHEN("register `r5` has value `0x1001` and the status flags set")
    {
      current.set_r5(0x1001);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r5(0x1008);
        expected.clear_status();
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r5` shall have the value `0x1008` and the zero and carry flags shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADC instruction using a right-hand short immediate `0x5` with register `r4`")
  {
    yarisc::test::machine current{
      yarisc::arch::assemble<opcode::add_with_carry>(r4, accumulator, short_immediate{0x5})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r4, r4, 5");
      }
    }

    WHEN("register `r4` has value `0xfffd` and the zero flag set")
    {
      current.set_r4(0xfffd);
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r4(0x0002);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("register `r4` shall have the value `0x0002` and only the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADC instruction using a left-hand immediate `0xf555` with registers `r2` and `r4`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add_with_carry>(r2, immediate, r4), 0xf555};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r2, 0xf555, r4");
      }
    }

    WHEN("register `r2` has value `0xfefe` and `r4` has value `0x0d00`")
    {
      current.set_r2(0xfefe);
      current.set_r4(0x0d00);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r2(0x0255);
        expected.set_status(yarisc::test::status_c);
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r2` shall have the value `0x0255` and the carry flag shall be set")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with an ADC instruction using a right-hand immediate `0x0203` with register `r3` and `r0`")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::add_with_carry>(r3, r0, immediate), 0x0203};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "ADC r3, r0, 0x0203");
      }
    }

    WHEN("register `r0` has value `0x1050`, `r3` has value `0xfefe`, and the status flags set")
    {
      current.set_r0(0x1050);
      current.set_r3(0xfefe);
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_r3(0x1254);
        expected.clear_status();
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("register `r3` shall have the value `0x1254` and the zero and carry flags shall be reset")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
