#pragma once
#include <string>

namespace  HackVMTranslator {
    class CodeWriter {
    public:
        explicit CodeWriter(std::ostream& outputStream) : outputStream_(outputStream) {}

        void writeArithmetic(const std::string& command);
        void writePush(const std::string& segment, int index) const;
        void writePop(const std::string& segment, int index) const;
        void writeInit();
        void writeLabel(const std::string& label) const;
        void writeGoto(const std::string& label) const;
        void writeIf(const std::string& label) const;
        void writeFunction(const std::string& name, int numVars);
        void writeCall(const std::string& name, int numVars);
        void writeReturn() const;
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
