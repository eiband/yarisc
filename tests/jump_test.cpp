/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the JMP instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a JMP to absolut short address `0x01fc` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::jump>(short_jump_address{0x01fc})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMP 0x01fc");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x01fc);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x01fc`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x01fc);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x01fc`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x01fc);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x01fc`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x01fc);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x01fc`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JMP to absolut short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::jump>(short_jump_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMP 0xffe0");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0xffe0);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0xffe0`")
      {
        CHECK(current == expected);
      }
    }
  }

  GIVEN("a test machine with a JMP to absolut address `0x6124` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::jump>(immediate), 0x6124};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMP 0x6124");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x6124);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x6124`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x6124);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x6124`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x6124);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x6124`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x6124);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x6124`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the JMC instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a JMC to absolut short address `0x1a` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jc, short_cond_jump_address{0x1a})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMC 0x001a");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip();

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall point to the next instruction")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JMC to absolut short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jc, short_cond_jump_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMC 0xffe0");
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0xffe0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0xffe0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JMC to absolut address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jc, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMC 0x1ff0");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip(2);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall point to the next instruction")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the JNC instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a JNC to absolut short address `0x1a` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnc, short_cond_jump_address{0x1a})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNC 0x001a");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x1a);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x1a`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JNC to absolut short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnc, short_cond_jump_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNC 0xffe0");
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0xffe0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0xffe0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JNC to absolut address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnc, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNC 0x1ff0");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x1ff0);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x1ff0`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the JMZ instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a JMZ to absolut short address `0x1a` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jz, short_cond_jump_address{0x1a})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMZ 0x001a");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip();

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall point to the next instruction")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JMZ to absolut short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jz, short_cond_jump_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMZ 0xffe0");
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0xffe0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0xffe0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JMZ to absolut address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jz, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JMZ 0x1ff0");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.advance_ip(2);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall point to the next instruction")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}

SCENARIO("execute the JNZ instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a JNZ to absolut short address `0x1a` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnz, short_cond_jump_address{0x1a})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNZ 0x001a");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x1a);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x1a`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1a);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1a`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip();

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JNZ to absolut short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnz, short_cond_jump_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNZ 0xffe0");
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0xffe0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0xffe0`")
        {
          CHECK(current == expected);
        }
      }
    }
  }

  GIVEN("a test machine with a JNZ to absolut address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_jump>(jnz, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "JNZ 0x1ff0");
      }
    }

    WHEN("the instruction is executed")
    {
      yarisc::test::machine expected = current;
      expected.set_ip(0x1ff0);

      REQUIRE(current.execute_instruction());

      THEN("the instruction pointer shall have the value `0x1ff0`")
      {
        CHECK(current == expected);
      }
    }

    WHEN("the carry flag is set")
    {
      current.set_status(yarisc::test::status_c);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the zero flag is set")
    {
      current.set_status(yarisc::test::status_z);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_zc);

      AND_WHEN("the instruction is executed")
      {
        yarisc::test::machine expected = current;
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }
  }
}
