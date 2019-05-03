#pragma once
#include <regex>

namespace HackVMTranslator {
    class Parser {
    public:
        enum class CommandType { ARITHMETIC, PUSH, POP, LABEL, GOTO, IF, FUNCTION, RETURN, CALL };

        explicit Parser(std::istream& inputStream) : inputStream_(inputStream) { updateNextCommand(); }

        bool hasMoreCommands() const { return !nextCommand_.empty(); }
        void advance();
        CommandType commandType() const;
        std::string arg1() const { return currentCommandArgs_[1]; }
        int arg2() const { return stoi(currentCommandArgs_[2]); }

    private:
        std::istream& inputStream_;
        std::string currentCommand_;
        std::smatch currentCommandArgs_;
        std::string nextCommand_;
        size_t currentLineNr_ = 0;
        CommandType currentCommandType_{};

        void updateNextCommand();
        void parseCurrentCommand();
    };
}


