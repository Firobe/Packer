This program is meant to read several generic shapes from a file in SVG format and execute a script in CE format, with the goal to move the shapes so that they take as less space as possible (bin packing problem), and outputs the result on the standard output.

# GNU/Linux instructions

#### Building

 * No dependencies required, only a C++11 complying compiler with OpenMP support.
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
 * Configure Cmake project : `cmake -DCMAKE_TOOLCHAIN_FILE=../CMakeModules/Toolchain-mingw-w64.cmake -DCMAKE_INSTALL_PREFIX=/usr/x86_64-w64-mingw32/ -Dtarget-os="windows" ..`. If your mingw package is located in a other path, you can specify the correct root directory of the toolchain in the CMAKE_INSTALL_PREFIX option.
 * Build it using `make`.
 * Executable name is `packer.exe`.

#### Installation

 * Copy files `packer.exe`, `packer.py` and `packer.in` to your Inkscape's extension folder.
 * It is usually located in `C:\users\:user_name:\appData\Roaming\Inkscape\extensions`.
 * `:user_name:` is the  name of your current windows session.

# Usage

#### Command-Line Interface

 * Basic usage : `./packer inputFile.svg > outputFile.svg` moves all the original shapes and pack them to fit the document's size, executing the default script `conf/default.ce`.
 * Another configuration file can be supplied with `--conf` or can be provided directly as a string with `--confString`.
 * Run `./packer --help` for a complete list of options.
 * `--width X` and `--height Y` can be used to specify the wanted size of bin (in px).
 * The `--dup [01]` enables the duplication of the original shapes, thus only the duplicates are moved.
 * The default behavior is to pack all shapes. If the user specifies IDs of shapes using `--id ID` (can be used multiple times), only those will be packed.
 * For the purpose of laser cutting, the `buffer X` can be used to force a minimal distance (in px) between the packed shapes.
 * If the project was built with graphical support, the flag `--display` can be used to enable a basic display of what is going on during the execution.
 * The option `--q PRECISION` can be specified to use QuadTrees instead of plain shapes. This can improve the performance of most algorithms. The higher the precision, the higher the performance (but a low precision yields better solutions.
 * Interrupting the program with C-c doesn't instantly stops the packing. Doing it once will make it stop as soon as possible and output a valid solution. Using C-c twice will force the interruption of the program.

#### Inkscape interface

 * Located in `Extensions->Packer` in the Inkscape menus.
 * The default behavior is to pack every shape. However if the user makes a selection of shapes beforehand, only those will be packed.
 * For now, the parameters can be specified in any unit Inkscape recognize.

# Scripting language

CloseEnough is a very simple DSL to facilitate the combination of the differents packing algorithms without having to rebuild the project. It supports function calls with parameters and the ability to repeat instruction blocks.

#### Usage

 * An instruction have the following pattern : function(param1=value1, param2=value2, ...);
 * Instructions must be followed by a semicolon
 * Multiple-instruction blocks are between a BEGIN and a END
 * Instruction blocks can be executed multiple times using DO X TIMES block

#### Available algorithms

 * `SimpleTransformer` : use brute force on randoms pair of shapes to merge them as close as possible, very slow. The following parameters can be supplied : `rotate_step` (an integer defining the used angle step, the higher the faster), `translate_nb` (the number of positions tried for each angle configuration, the higher the slower), `rentability` (the treshold used to determine if the gain is sufficient to merge two shapes). The parameter `intersection` MUST be supplied and can have two values : `intersection` or `box`.
 * `HoleTransformer` : a basic algorithm that tries for each known hole to fit another shape in it.
 * `LineSolver` : places every shape on a single line.
 * `MultilineSolver` : places every shape on multiple lines.
 * `ScanlineSolver` : uses the first-fit algorithm to pack shapes. Very efficient on rectangular shapes.
 * `FreezeSolver` : uses a physics-like algorithm to pack one shape after the other with gravity.
 * `ProbaSolver` : a probabilist solver using random steps. The parameter `amplitudeProba` can be provided. It will specify the amplitude of the random movements (a higher number means smaller steps).
 * `TaskSolver` : uses multiple instances of ProbaSolver and keeps only the best solution.

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
