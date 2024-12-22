# GHChecksum
Small command-line program used to help translate QBKey checksums back to their original value using a .checksums table.

Designed for QBC syntax scripts

# How to build
The only requirements to build the library and program is just CMake and any compiler with support for C++17.
*(e.g. Visual C++, GCC, Clang)*

1. Using a terminal or command prompt, create a new folder inside the root directory which will be used by CMake then simply invoke the following command:
`cmake ..`

This should generate necessary files to allow for building. By default, the library is set to compile as a static library, but if you want to instead compile as a shared library (appears in the form of a .DLL file on Windows, .dylib on Mac, or .so on Linux), simply invoke this command:
`cmake -DGHCHECKSUM_COMPILE_AS_SHAREDLIB=ON ..`

2. While still inside the folder used for building, simply use whichever generated build you decided to use (The default in Windows, for instance is a VS solution, which in that case you can just use Visual Studio to compile the program and library)
