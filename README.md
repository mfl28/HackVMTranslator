# HackVMTranslator [![Build Status](https://travis-ci.org/mfl28/HackVMTranslator.svg?branch=master)](https://travis-ci.org/mfl28/HackVMTranslator) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/mfl28/HackVMTranslator.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/mfl28/HackVMTranslator/context:cpp)
This is a C++-implementation of a translator for the Hack-platform's virtual-machine language as defined in [project 7](https://www.nand2tetris.org/project07) and [project 8](https://www.nand2tetris.org/project08) of the nand2tetris-course.
The program takes as argument either a single `.vm`-file or a directory containing such files, translates them into Hack assembly-language code and writes this code into a single `.asm`-file.

## Building the program
_Note_: To build this program, a compiler that supports C++17 and `std::filesystem` is required (e.g. gcc version &geq; 8).

```bash
git clone https://github.com/mfl28/HackVMTranslator.git
cd HackVMTranslator
cmake -B build    # Use option "-D BUILD_TESTING=OFF" if you do not want to build the unit-tests.
cmake --build build   
```
## Running the program
After you built the program, do the following from within the `build`-directory:
#### Linux
```bash
./HackVMTranslator path/to/filename.vm    # Or "./HackVMTranslator path/to/directory"
```
#### Windows
```bash
cd Debug    # Or "cd Release" if you built using Release-configuration.
.\HackVMTranslator.exe path\to\filename.vm    # Or ".\HackVMTranslator path\to\directory"
```
## Running the tests
If you built the program including the unit-tests, then these can be run from within the `build`-directory by doing the following:
#### Linux
```bash
ctest -V
```
#### Windows
```bash
ctest -C Debug -V    # Or "ctest -C Release -V" if you built using Release-configuration.
```
## References
- [nand2tetris-course](https://www.nand2tetris.org)
- [Google Test](https://github.com/google/googletest)
