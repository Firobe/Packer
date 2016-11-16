COMPILER = c++
CXXFLAGS = -Wall -Wfatal-errors -std=c++11 -isystem third_party -O2
LIBS =
EXEC_NAME = packer
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXAMPLE_FILE = vertebrae.svg
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LIBS)

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: all
	./$(EXEC_NAME) examples/$(EXAMPLE_FILE) > output.svg
	#Output stored in output.svg
	firefox output.svg &

format:
	astyle -RA2fpHUxC90xLyjk1W1 "src/*.cpp" "src/*.hpp"
	rm -Rf src/*.orig

clean:
	rm -f $(OBJECTS) $(EXEC_NAME) output.svg
