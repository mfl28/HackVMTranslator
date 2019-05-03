#include "Parser.h"
#include <gtest/gtest.h>
#include <sstream>

using std::stringstream;
using std::istream;
using std::ostream;
using std::string;
using HackVMTranslator::Parser;

namespace {
    TEST(ParserTest, HasMoreCommands) {
        stringstream s{"// test program 1  \n push constant 1  \n //... \n push constant 1 \n add"};
        Parser parser{s};

        ASSERT_TRUE(parser.hasMoreCommands());

        parser.advance();
        ASSERT_TRUE(parser.hasMoreCommands());

        parser.advance();
        ASSERT_TRUE(parser.hasMoreCommands());

        parser.advance();
        ASSERT_FALSE(parser.hasMoreCommands());
    }

    TEST(ParserTest, CommandsCorrectlyParsed) {
        stringstream s{"push constant 1 \n push constant 7 \n add \n pop local 3"};
        Parser parser{s};

        parser.advance();
        ASSERT_EQ(parser.commandType(), Parser::CommandType::PUSH);
        ASSERT_EQ(parser.arg1(), "constant");
        ASSERT_EQ(parser.arg2(), 1);

        parser.advance();
        ASSERT_EQ(parser.commandType(), Parser::CommandType::PUSH);
        ASSERT_EQ(parser.arg1(), "constant");
        ASSERT_EQ(parser.arg2(), 7);

        parser.advance();
        ASSERT_EQ(parser.commandType(), Parser::CommandType::ARITHMETIC);
        ASSERT_EQ(parser.arg1(), "add");

        parser.advance();
        ASSERT_EQ(parser.commandType(), Parser::CommandType::POP);
        ASSERT_EQ(parser.arg1(), "local");
        ASSERT_EQ(parser.arg2(), 3);

        ASSERT_FALSE(parser.hasMoreCommands());
    }
}
