#include "HackVMTranslator.h"
#include "Parser.h"
#include "CodeWriter.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

using std::string;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::runtime_error;
using std::stringstream;
using std::endl;
using std::istream;

namespace fs = std::filesystem;

namespace HackVMTranslator {
    namespace {
        void translateFile(CodeWriter& codeWriter, istream& inputFile) {
            Parser parser(inputFile);

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

    int translate(const string& inputPathName) {
        const fs::path inputPath(inputPathName);

        if(!is_directory(inputPath) && inputPath.extension() != ".vm") {
            cout << "Invalid argument: Must be either a path to a *.vm file or a path to a directory (containing *.vm files)." << endl;
            return -1;
        }

        stringstream outputBuffer;
        CodeWriter codeWriter{outputBuffer};

        if(is_directory(inputPath)) {
            codeWriter.writeInit();
            size_t validFiles{};
            auto sysFilePresent{false};

            for(const auto& item : fs::directory_iterator{inputPath}) {
                if(item.path().extension() == ".vm") {
                    if(ifstream file{item.path()}) {
                        codeWriter.setFileName(item.path().stem().string());

                        try {
                            translateFile(codeWriter, file);
                        }
                        catch(const runtime_error& e) {
                            cout << "VMTranslator error in file " << item.path().filename() << ": " << e.what() << endl;
                            return -1;
                        }
                    }
                    else {
                        cout << "VMTranslator error: Could not open file " << item.path().filename() << '.' << endl;
                        return -1;
                    }

                    if(!sysFilePresent && item.path().filename() == "Sys.vm") {
                        sysFilePresent = true;
                    }

                    ++validFiles;
                }
            }

            if(validFiles == 0) {
                cout << "VMTranslator error: The directory " << inputPath << " does not contain any *.vm files." << endl;
                return -1;
            }

            if(!sysFilePresent) {
                cout << "VMTranslator error: Sys.vm was not found in directory " << inputPath << '.' << endl;
                return -1;
            }
        }
        else if(ifstream file{inputPath}) {
            codeWriter.setFileName(inputPath.filename().stem().string());

            try {
                translateFile(codeWriter, file);
            }
            catch(const runtime_error& e) {
                cout << "VMTranslator error: " << e.what() << endl;
                return -1;
            }
        }
        else {
            cout << "VMTranslator error: Could not open file " << inputPath.filename() << '.' << endl;
            return -1;
        }

        auto outputPath = is_directory(inputPath) ? inputPath / inputPath.stem() : inputPath;
        outputPath.replace_extension(".asm");

        if(ofstream outputFile{outputPath}) {
            outputFile << outputBuffer.str();
        }
        else {
            cout << "VMTranslator error: Could not create output file " << outputPath << '.' << endl;
            return -1;
        }

        return 0;
    }
}