#pragma once
#include <regex>

namespace HackVMTranslator {
    class Parser {
    public:
        /**
         * \brief Contains the different Hack VM-language command-types.
         */
        enum class CommandType { ARITHMETIC, PUSH, POP, LABEL, GOTO, IF, FUNCTION, RETURN, CALL };

        /**
         * \brief Creates a new parser for a provided input-stream and gets ready to parse the
         * first command (if one exists).
         * \param inputStream The stream to be parsed
         */
        explicit Parser(std::istream& inputStream) : inputStream_(inputStream) { updateNextCommand(); }
        /**
         * \brief Checks if there exists another command in the input-stream.
         * \return True if another command exists, otherwise false.
         */
        bool hasMoreCommands() const { return !nextCommand_.empty(); }

        /**
         * \brief Sets the current command to the next command encountered
         * in the input-stream. Must only be called if hasMoreCommands()
         * returns true.
         */
        void advance();

        /**
         * \brief Gets the type of the current command.
         * \return A VM-language command-type
         */
        CommandType commandType() const;

        /**
         * \brief Gets the first argument of the current command. Must not be called
         * if the current command has type RETURN.
         * \return The command-name itself if the current command's type is ARITHMETIC, otherwise
         * the first argument of the current command 
         */
        std::string arg1() const { return currentCommandArgs_[1]; }


        /**
         * \brief Gets the second argument of current command. Must only be called
         * if the current command has type PUSH, POP, FUNCTION or CALL.
         * \return The second argument.
         */
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


