/*
 * This file is part of the YaRISC processor project which is released under the MIT license.
 * See file LICENSE in the root folder of this repository for details.
 */

#include <catch2/catch_test_macros.hpp>
#include <tests/machine.hpp>
#include <yarisc/arch/assembly.hpp>

#include <string>

SCENARIO("execute the BRA instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BRA to absolute short address `0x01fc` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::branch>(short_branch_address{0x01fc})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BRA 0x01fc");
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
      current.set_status(yarisc::test::status_nzcv);

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

  GIVEN("a test machine with a BRA to absolute short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::branch>(short_branch_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BRA 0xffe0");
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

  GIVEN("a test machine with a BRA to absolute address `0x6124` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::branch>(immediate), 0x6124};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BRA 0x6124");
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
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BEQ instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BEQ to absolute short address `0x1a` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(eq, short_cond_branch_address{0x1a})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BEQ 0x001a");
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

  GIVEN("a test machine with a BEQ to absolute short address `0xffe0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(eq, short_cond_branch_address{0xffe0})};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction();

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BEQ 0xffe0");
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

  GIVEN("a test machine with a BEQ to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(eq, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BEQ 0x1ff0");
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BLT instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BLT to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(lt, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BLT 0x1ff0");
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

    WHEN("the negative flag is set")
    {
      current.set_status(yarisc::test::status_n);

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

    WHEN("the overflow flag is set")
    {
      current.set_status(yarisc::test::status_v);

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

    WHEN("the negative and overflow flags are set")
    {
      current.set_status(yarisc::test::status_nv);

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
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BLE instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BLE to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(le, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BLE 0x1ff0");
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

    WHEN("the negative flag is set")
    {
      current.set_status(yarisc::test::status_n);

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
        expected.set_ip(0x1ff0);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall have the value `0x1ff0`")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the overflow flag is set")
    {
      current.set_status(yarisc::test::status_v);

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

    WHEN("the negative and overflow flags are set")
    {
      current.set_status(yarisc::test::status_nv);

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
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BLO instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BLO to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(lo, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BLO 0x1ff0");
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

    WHEN("the zero and carry flags are set")
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BLS instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BLS to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(ls, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BLS 0x1ff0");
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

    WHEN("the zero and carry flags are set")
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BNE instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BNE to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(ne, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BNE 0x1ff0");
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BGE instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BGE to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(ge, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BGE 0x1ff0");
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

    WHEN("the negative flag is set")
    {
      current.set_status(yarisc::test::status_n);

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

    WHEN("the overflow flag is set")
    {
      current.set_status(yarisc::test::status_v);

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

    WHEN("the negative and overflow flags are set")
    {
      current.set_status(yarisc::test::status_nv);

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
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BGT instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BGT to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(gt, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BGT 0x1ff0");
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

    WHEN("the negative flag is set")
    {
      current.set_status(yarisc::test::status_n);

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
        expected.advance_ip(2);

        REQUIRE(current.execute_instruction());

        THEN("the instruction pointer shall point to the next instruction")
        {
          CHECK(current == expected);
        }
      }
    }

    WHEN("the overflow flag is set")
    {
      current.set_status(yarisc::test::status_v);

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

    WHEN("the negative and overflow flags are set")
    {
      current.set_status(yarisc::test::status_nv);

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
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BHS instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BHS to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(hs, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BHS 0x1ff0");
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

    WHEN("the zero and carry flags are set")
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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

SCENARIO("execute the BHI instruction", "[instruction]")
{
  using namespace yarisc::arch::assembly;

  GIVEN("a test machine with a BHI to absolute address `0x1ff0` instruction")
  {
    yarisc::test::machine current{yarisc::arch::assemble<opcode::cond_branch>(hi, immediate), 0x1ff0};

    WHEN("the instruction is disassembled")
    {
      const std::string text = current.disassemble_instruction(2);

      THEN("the result shall be the expected text")
      {
        CHECK(text == "BHI 0x1ff0");
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

    WHEN("the zero and carry flags are set")
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

    WHEN("the status flags are set")
    {
      current.set_status(yarisc::test::status_nzcv);

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
