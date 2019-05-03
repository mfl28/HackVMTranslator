#include "Parser.h"
#include "CodeWriter.h"
#include <gtest/gtest.h>
#include <sstream>

using std::stringstream;
using std::istream;
using std::ostream;
using std::string;
using HackVMTranslator::Parser;
using HackVMTranslator::CodeWriter;

namespace {
    void performTranslation(istream& input, ostream& output);

    TEST(CodeWriterTest, SinglePush) {
        stringstream input{"push constant 1"}, output;
        const string expectedOutput{"@1\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n"};

        performTranslation(input, output);

        ASSERT_EQ(output.str(), expectedOutput);
    }

    void performTranslation(istream& input, ostream& output) {
        Parser parser{input};
        CodeWriter codeWriter{output};

        while(parser.hasMoreCommands()) {
            parser.advance();

            switch(parser.commandType()) {
                case Parser::CommandType::ARITHMETIC:
                    codeWriter.writeArithmetic(parser.arg1());
                    break;
                case Parser::CommandType::PUSH:
                    codeWriter.writePush(parser.arg1(), parser.arg2());
                    break;
                case Parser::CommandType::POP:
                    codeWriter.writePop(parser.arg1(), parser.arg2());
                    break;
                case Parser::CommandType::LABEL:
                    codeWriter.writeLabel(parser.arg1());
                    break;
                case Parser::CommandType::GOTO:
                    codeWriter.writeGoto(parser.arg1());
                    break;
                case Parser::CommandType::IF:
                    codeWriter.writeIf(parser.arg1());
                    break;
                case Parser::CommandType::FUNCTION:
                    codeWriter.writeFunction(parser.arg1(), parser.arg2());
                    break;
                case Parser::CommandType::RETURN:
                    codeWriter.writeReturn();
                    break;
                case Parser::CommandType::CALL:
                    codeWriter.writeCall(parser.arg1(), parser.arg2());
            }
        }
    }
}