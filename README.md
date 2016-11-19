# Build an executable for Windows (x86_64 processors instruction set) : debian instructions

 * Require package `mingw-w64`
 * Create build directory `mkdir build-win`
 * Go to it `cd build-win`
 * Configure Cmake project : `cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw-w64.cmake -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32/ -Dtarget-os="windows" ..`
 * build it `make`
 * Executable name is `packer.exe`
