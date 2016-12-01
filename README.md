# Windows instructions

#### Build an executable for Windows (x86_64 processors instruction set) : on a debian system

 * Require package `mingw-w64`
 * Create build directory `mkdir build-win`
 * Go to it `cd build-win`
 * Configure Cmake project : `cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw-w64.cmake -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32/ -Dtarget-os="windows" ..`
 * build it `make`
 * Executable name is `packer.exe`

#### Installation : on a Windows system

 * Copy files `packer.exe` and `packer.in` to your Inkscape's extension folder.
 * It's usually located in `C:\users\:user_name:\appData\Roaming\Inkscape\extensions`.
 * `:user_name:` is the  name of your current windows session.

# Generate documentation

 * Doxygen is required
 * Just run the command `make doc` from the build folder
 * Open the doc/html/index.html to view it
 
 
 * Open Mp
 * RabbitMQ