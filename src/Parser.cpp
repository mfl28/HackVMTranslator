#include "Parser.h"
#include <algorithm>
#include <vector>

using std::istream;
using std::string;
using std::regex;
using std::runtime_error;
using std::vector;
using std::pair;
using std::to_string;

namespace HackVMTranslator {
    namespace {
        const vector<pair<Parser::CommandType, regex>> COMMAND_TYPE_TO_PATTERN{
            { Parser::CommandType::ARITHMETIC, regex{"^(add|sub|neg|eq|gt|lt|and|or|not)$", regex::optimize} },
            { Parser::CommandType::PUSH,       regex{R"(^push (local|argument|this|that|constant|static|temp|pointer) (\d+)$)", regex::optimize} },
            { Parser::CommandType::POP,        regex{R"(^pop (local|argument|this|that|static|temp|pointer) (\d+)$)", regex::optimize} },
            { Parser::CommandType::LABEL,      regex{R"(^label ([\w:\.]+)$)", regex::optimize} },
            { Parser::CommandType::GOTO,       regex{R"(^goto ([\w:\.]+)$)", regex::optimize} },
            { Parser::CommandType::IF,         regex{R"(^if-goto ([\w:\.]+)$)", regex::optimize} },
            { Parser::CommandType::FUNCTION,   regex{R"(^function ([\w\.]+) (\d+)$)", regex::optimize} },
            { Parser::CommandType::RETURN,     regex{"^return$", regex::optimize} },
            { Parser::CommandType::CALL,       regex{R"(^call ([\w\.]+) (\d+)$)", regex::optimize} }
        };

        void trimWhiteSpaceAndComments(string& line) {
            // (1) trim possible inline comments
            line = line.substr(0, line.find("//"));
            // (2) trim all leading and trailing whitespaces/tabs
            if(const auto firstNonWhiteSpaceIt = line.find_first_not_of(" \t"); firstNonWhiteSpaceIt != string::npos) {
                line = line.substr(firstNonWhiteSpaceIt, line.find_last_not_of(" \t") - firstNonWhiteSpaceIt + 1);
            }
            else {
                line = "";
            }
            // (3) contract multiple whitespaces between words to a single whitespace each
            const auto uniqueEnd = unique(line.begin(), line.end(), [] (auto lhs, auto rhs) { return (lhs == rhs) && (lhs == ' '); });
            line.erase(uniqueEnd, line.end());
        }
    }

    void Parser::updateNextCommand() {
        string line;

        while(inputStream_ && line.empty()) {
            getline(inputStream_, line);
            trimWhiteSpaceAndComments(line);
            ++currentLineNr_;
        }

        nextCommand_ = line;
    }

    void Parser::advance() {
        currentCommand_ = nextCommand_;
        parseCurrentCommand();
        updateNextCommand();
    }

    void Parser::parseCurrentCommand() {
        if(const auto it = find_if(COMMAND_TYPE_TO_PATTERN.cbegin(), COMMAND_TYPE_TO_PATTERN.cend(),
           [&] (const auto& item) { return regex_match(currentCommand_, currentCommandArgs_, item.second); });
           it != COMMAND_TYPE_TO_PATTERN.cend()) {
           currentCommandType_ = it->first;
        }
        else {
            throw runtime_error("Invalid syntax in line " + to_string(currentLineNr_) + ": " + currentCommand_);
        }
    }

    Parser::CommandType Parser::commandType() const {
        return currentCommandType_;
    }
}
