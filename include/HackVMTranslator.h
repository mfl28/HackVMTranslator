#pragma once
#include <string>

namespace HackVMTranslator {
    /**
     * \brief Translates one or more .vm files containing Hack-platform-VM-language code
     * into a single .asm file containing the concatenated translations of the input-file(s)
     * into Hack assembly code. 
     * \param inputPathName The path to a .vm file or the path to a directory containing
     * .vm files.
     * \return 0 if the translation was successful, -1 otherwise.
     */
    int translate(const std::string& inputPathName);
}


