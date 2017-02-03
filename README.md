# GNU/Linux instructions

#### Building

 * No dependencies required, only a C++11 complying compiler.
 * Create a build directory and enter it `mkdir build && cd build`.
 * Generate Makefiles using `cmake ..`.
 * Build the project using `make` (multithreaded compilation using `-j <coreNb>` is advised).
 * Binary is named `packer`.
 * `make test` will run the tests.
 * `make doc` will generated the documentation (Doxygen required) into `build/doc/html/`.

#### Installation

 * Directory `~/.config/inkscape/extensions/` must exist.
 * Run `make install` to install packer as an Inkscape extension.


# Windows instructions

#### Building for Windows (x86_64) on a Debian system

 * Require package `mingw-w64`.
 * Create a build directory and enter it `mkdir build-win && cd build-win`.
 * Configure Cmake project : `cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw-w64.cmake -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32/ -Dtarget-os="windows" ..`.
 * Build it using `make`.
 * Executable name is `packer.exe`.

#### Installation

 * Copy files `packer.exe` and `packer.in` to your Inkscape's extension folder.
 * It is usually located in `C:\users\:user_name:\appData\Roaming\Inkscape\extensions`.
 * `:user_name:` is the  name of your current windows session.

# Usage

#### Command-Line Interface

 * Basic usage : `./packer inputFile.svg > outputFile.svg` moves all the original shapes and pack them to fit the document's size.
 * Run `./packer --help` for a complete list of options.
 * `--width X` and `--height Y` can be used to specify the wanted size of bin (in px).
 * The `--dup [01]` enables the duplication of the original shapes, thus only the duplicates are moved.
 * The default behavior is to pack all shapes. If the user specifies IDs of shapes using `--id ID` (can be used multiple times), only those will be packed.
 * For the purpose of laser cutting, the `buffer X` can be used to force a minimal distance (in px) between the packed shapes.

#### Inkscape interface

 * Located in `Extensions->Packer` in the Inkscape menus.
 * The default behavior is to pack every shape. However if the user makes a selection of shapes beforehand, only those will be packed.
 * For now, the parameters must be specified in pixels.

# Features

#### Processed SVG features

 * Every `path` description.
 * Structural elements `svg` and `g`.
 * Primitives `rect`, `ellipse`, `line`, `polygon`, `polyline` and `circle`.
 * Attributes `id`, `height`, `width` and `transform` (every format).

#### Grouping policy

 * A shape can contain multiple paths that will be moved jointly or a single path.
 * Only the most atomic SVG groups (i.e. groups without subgroups) will be considered as shapes. Paths located outside of such groups will be considered as shapes containing a single path.
 * Inside a multiple-path shape, a path completely covered by another will be processed as a hole.
