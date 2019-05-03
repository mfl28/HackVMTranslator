#include "CodeWriter.h"
#include <sstream>
#include <algorithm>
#include <unordered_map>

using std::unordered_map;
using std::string;
using std::stringstream;

namespace HackVMTranslator {
    namespace {
        const string UNARY_COMMAND_FETCH_CODE{
            "@SP\n"
            "A=M-1\n"
        };

        const string POP_TO_D_AND_DECREMENT_SP_CODE{
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
        };

        const string BINARY_COMMAND_FETCH_CODE{
            POP_TO_D_AND_DECREMENT_SP_CODE +
            "A=A-1\n"
        };


        const string WRITE_D_TO_STACK_AND_UPDATE_SP_CODE{
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n"
        };

        const unordered_map<string, string> NON_COMPARISON_ARITHMETIC_COMMAND_TO_CODE{
            {"add", BINARY_COMMAND_FETCH_CODE + "M=D+M\n"},
            {"sub", BINARY_COMMAND_FETCH_CODE + "M=M-D\n"},
            {"and", BINARY_COMMAND_FETCH_CODE + "M=D&M\n"},
            {"or",  BINARY_COMMAND_FETCH_CODE + "M=D|M\n"},
            {"neg", UNARY_COMMAND_FETCH_CODE + "M=-M\n"},
            {"not", UNARY_COMMAND_FETCH_CODE + "M=!M\n"}
        };

        const unordered_map<string, string> SEGMENT_TO_REGISTER_NAME{
            {"local",    "LCL"},
            {"argument", "ARG"},
            {"this",     "THIS"},
            {"that",     "THAT"}
        };
    }

    void CodeWriter::writeArithmetic(const std::string& command) {
        if(const auto it = NON_COMPARISON_ARITHMETIC_COMMAND_TO_CODE.find(command); it != NON_COMPARISON_ARITHMETIC_COMMAND_TO_CODE.cend()) {
            outputStream_ << it->second;
        }
        else {
            writeArithmeticComparisonCommandCode(command);
            ++currentComparisonCommandId_;
        }
    }

    void CodeWriter::writeArithmeticComparisonCommandCode(string command) const {
        transform(command.begin(), command.end(), command.begin(), toupper);

        outputStream_ << BINARY_COMMAND_FETCH_CODE <<
            "D=M-D\n"
            "@" << command << "_TRUE_" << currentComparisonCommandId_ << "\n"
            "D;J" << command << "\n"
            << UNARY_COMMAND_FETCH_CODE <<
            "M=0\n"
            "@" << command << "_END_" << currentComparisonCommandId_ << "\n"
            "0;JMP\n"
            "(" << command << "_TRUE_" << currentComparisonCommandId_ << ")\n"
            << UNARY_COMMAND_FETCH_CODE <<
            "M=-1\n"
            "(" << command << "_END_" << currentComparisonCommandId_ << ")\n";
    }

    void CodeWriter::writePush(const std::string & segment, int index) const {
        if(const auto it = SEGMENT_TO_REGISTER_NAME.find(segment); it != SEGMENT_TO_REGISTER_NAME.end()) {
            if(index > 1) {
                outputStream_ << "@" << index << "\n"
                    "D=A\n"
                    "@" << it->second << "\n"
                    "A=D+M\n"
                    "D=M\n";
            }
            else {
                outputStream_ << "@" << it->second << "\n"
                    "A=M" << (index == 1 ? "+1" : "") << "\n"
                    "D=M\n";
            }
        }
        else if(segment == "pointer") {
            outputStream_ << "@" << (index == 0 ? "THIS" : "THAT") << "\n"
                "D=M\n";
        }
        else if(segment == "temp") {
            outputStream_ << "@" << 5 + index << "\n"
                "D=M\n";
        }
        else if(segment == "constant") {
            outputStream_ << "@" << index << "\n"
                "D=A\n";
        }
        else {
            //static
            outputStream_ << "@" << currentFileName_ << "." << index << "\n"
                "D=M\n";
        }

        outputStream_ << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE;
    }

    void CodeWriter::writePop(const std::string & segment, int index) const {
        if(const auto it = SEGMENT_TO_REGISTER_NAME.find(segment); it != SEGMENT_TO_REGISTER_NAME.end()) {
            if(index > 1) {
                outputStream_ << "@" << index << "\n"
                    "D=A\n"
                    "@" << it->second << "\n"
                    "D=D+M\n"
                    "@R13\n"
                    "M=D\n"
                    << POP_TO_D_AND_DECREMENT_SP_CODE <<
                    "@R13\n"
                    "A=M\n"
                    "M=D\n";
            }
            else {
                outputStream_ << POP_TO_D_AND_DECREMENT_SP_CODE
                    << "@" << it->second << "\n"
                    "A=M" << (index == 1 ? "+1" : "") << "\n"
                    "M=D\n";
            }
        }
        else if(segment == "pointer") {
            outputStream_ << POP_TO_D_AND_DECREMENT_SP_CODE <<
                "@" << (index == 0 ? "THIS" : "THAT") << "\n"
                "M=D\n";
        }
        else if(segment == "temp") {
            outputStream_ << POP_TO_D_AND_DECREMENT_SP_CODE <<
                "@" << 5 + index << "\n"
                "M=D\n";
        }
        else if(segment == "static") {
            outputStream_ << POP_TO_D_AND_DECREMENT_SP_CODE <<
                "@" << currentFileName_ << "." << index << "\n"
                "M=D\n";
        }
    }

    void CodeWriter::writeInit() {
        outputStream_ << "@256\n"
            "D=A\n"
            "@SP\n"
            "M=D\n";
        writeCall("Sys.init", 0);
    }

    void CodeWriter::writeLabel(const string& label) const {
        outputStream_ << "(" << currentFunctionName_ << "$" << label << ")\n";
    }

    void CodeWriter::writeGoto(const string& label) const {
        outputStream_ << "@" << currentFunctionName_ << "$" << label << "\n"
            "0;JMP\n";
    }

    void CodeWriter::writeIf(const string& label) const {
        outputStream_ << "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@" << currentFunctionName_ << "$" << label << "\n"
            "D;JNE\n";
    }

    void CodeWriter::writeFunction(const string& name, int numVars) {
        currentFunctionName_ = name;

        outputStream_ << "(" << currentFunctionName_  << ")\n"
            "@SP\n"
            "A=M\n";

        for(auto i = 0; i != numVars; ++i) {
            outputStream_ << "M=0\n"
                "A=A+1\n";
        }

        outputStream_ << "D=A\n"
            "@SP\n"
            "M=D\n";
    }

    void CodeWriter::writeCall(const string& name, int numVars) {
        outputStream_ << "@" << currentFunctionName_ << "$ret." << currentReturnSymbolId_ << "\n"
            "D=A\n"
            << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE <<
            "@LCL\n"
            "D=M\n"
            << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE <<
            "@ARG\n"
            "D=M\n"
            << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE <<
            "@THIS\n"
            "D=M\n"
            << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE <<
            "@THAT\n"
            "D=M\n"
            << WRITE_D_TO_STACK_AND_UPDATE_SP_CODE <<
            "@" << numVars + 5 << "\n"
            "D=A\n"
            "@SP\n"
            "D=M-D\n"
            "@ARG\n"
            "M=D\n"
            "@SP\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            << "@" << name << "\n"
            "0;JMP\n"
            << "(" << currentFunctionName_ << "$ret." << currentReturnSymbolId_ << ")\n";

        ++currentReturnSymbolId_;
    }

    void CodeWriter::writeReturn() const {
        outputStream_ << "@LCL\n"
            "D=M\n"
            "@R14\n"
            "M=D\n"
            "@5\n"
            "D=A\n"
            "@R14\n"
            "A=M-D\n"
            "D=M\n"
            "@R15\n"
            "M=D\n";

        writePop("argument", 0);

        outputStream_ << "@ARG\n"
            "D=M+1\n"
            "@SP\n"
            "M=D\n"
            "@R14\n"
            "AM=M-1\n"
            "D=M\n"
            "@THAT\n"
            "M=D\n"
            "@R14\n"
            "AM=M-1\n"
            "D=M\n"
            "@THIS\n"
            "M=D\n"
            "@R14\n"
            "AM=M-1\n"
            "D=M\n"
            "@ARG\n"
            "M=D\n"
            "@R14\n"
            "AM=M-1\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            "@R15\n"
            "A=M\n"
            "0;JMP\n";
    }
}