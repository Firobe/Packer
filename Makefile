COMPILER = c++
ADD_CFLAGS = -Wall -std=c++11 -O3
LIBS =
CXXFLAGS =
SOURCES = $(wildcard src/*.cpp)
EXEC_NAME = packer
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
