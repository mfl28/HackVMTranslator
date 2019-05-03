#pragma once
#include <string>

namespace  HackVMTranslator {
    class CodeWriter {
    public:
        /**
         * \brief Creates a new CodeWriter object that can be used write Hack assembly code
         * to a provided output-stream.
         * \param outputStream
         */
        explicit CodeWriter(std::ostream& outputStream) : outputStream_(outputStream) {}

        /**
         * \brief Writes assembly code corresponding to the translation of an ARITHMETIC-command.
         * \param command 
         */
        void writeArithmetic(const std::string& command);

        /**
         * \brief Writes assembly code corresponding to the translation of a PUSH-command.
         * \param segment The RAM-segment from which to push a value (or "constant").
         * \param index The index in the segment of the value to be pushed (or just the value
         * to be pushed in case of the segment "constant").
         */
        void writePush(const std::string& segment, int index) const;

        /**
         * \brief Writes assembly code corresponding to the translation of a POP-command.
         * \param segment The RAM-segment onto which the top-stack value should be popped.
         * \param index The index in the segment into which the value should be placed.
         */
        void writePop(const std::string& segment, int index) const;

        /**
         * \brief Writes assembly code corresponding to the initial setup of a program.
         */
        void writeInit();

        /**
         * \brief Writes assembly code corresponding to the translation of a label definition.
         * \param label 
         */
        void writeLabel(const std::string& label) const;

        /**
         * \brief Writes assembly code corresponding to the translation of a goto-command.
         * \param label The target of the goto.
         */
        void writeGoto(const std::string& label) const;

        /**
         * \brief Writes assembly code corresponding to the translation of an if-goto-command.
         * \param label The target of the if-goto.
         */
        void writeIf(const std::string& label) const;

        /**
         * \brief Writes assembly code corresponding to the translation of a function declaration.
         * \param name The name of the function.
         * \param numLocals The number of local variables of the function.
         */
        void writeFunction(const std::string& name, int numLocals);

        /**
         * \brief Writes assembly code corresponding to the translation of a function call.
         * \param name The name of the function to be called.
         * \param numArgs The number of arguments of the function.
         */
        void writeCall(const std::string& name, int numArgs);

        /**
         * \brief Writes assembly code corresponding to the translation of a return from a function.
         */
        void writeReturn() const;

        /**
         * \brief Informs the code writer that the translation of
         * a new VM file is started. 
         * \param fileName The new file's name
         */
        void setFileName(const std::string& fileName) { currentFileName_ = fileName; }

    private:
        std::ostream& outputStream_;
        std::string currentFileName_;
        size_t currentComparisonCommandId_{};
        size_t currentReturnSymbolId_{};
        std::string currentFunctionName_;

        void writeArithmeticComparisonCommandCode(std::string command) const;
    };
}
