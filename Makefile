COMPILER = c++
CXXFLAGS = -Wall -Wfatal-errors -std=c++11 -isystem third_party -O2
LIBS = -lboost_program_options
LIBS_DIR = -Lthird_party/boost/stage/lib/
EXEC_NAME = packer
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXAMPLE_FILE = vertebrae.svg
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(LIBS_DIR) $(OBJECTS) $(LIBS)

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: all
	./$(EXEC_NAME) examples/$(EXAMPLE_FILE) > output.svg
	#Output stored in output.svg
	firefox output.svg &

format:
	astyle -RA2fpHUxC90xLyjk1W1 "src/*.cpp" "src/*.hpp"
	rm -Rf src/*.orig

program_options:
	cd third_party/boost && sh bootstrap.sh --with-libraries=program_options
	cd third_party/boost && ./b2

clean:
	rm -f $(OBJECTS) $(EXEC_NAME) output.svg
