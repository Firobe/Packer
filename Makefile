COMPILER = c++
CXXFLAGS = -Wall -std=c++11 -O2 -isystem third_party
LIBS =
EXEC_NAME = packer
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LIBS)

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: all
	./$(EXEC_NAME)

format:
	astyle -RA2fpHUxC90xLyjk1W1 "*.cpp" "*.h"
	rm -Rf src/*.orig

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
