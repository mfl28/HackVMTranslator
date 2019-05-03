#include "HackVMTranslator.h"
#include <iostream>

using std::cout;
using std::flush;

int main(int argc, char** argv)
{
    if(argc == 2) {
        return HackVMTranslator::translate(argv[1]);
    }

    cout << "Wrong number of arguments. Usage: HackVMTranslator <<filename>.vm OR <directoryName>>" << flush;
    return -1;
}
