COMPILER = c++
ADD_CFLAGS = -Wall -std=c++11 -O3
LIBS =
NO_PKG_LIBS =
CXXFLAGS = `pkg-config --static --cflags $(LIBS)` $(ADD_CFLAGS)
LINKS = `pkg-config --static --libs $(LIBS)` $(NO_PKG_LIBS)
SOURCES = $(wildcard src/*.cpp)
EXEC_NAME = packer
OBJECTS = $(SOURCES:.cpp=.o)
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: $(OBJECTS)
	$(COMPILER) -o $(EXEC_NAME) $(OBJECTS) $(LINKS)

%.o : %.cpp %.h
	$(COMPILER) -c $(CXXFLAGS) $< -o $@

run: all
	./$(EXEC_NAME)

format:
	astyle -RA2fpHUxLyxj "*.cpp" "*.h"
	rm -R *.orig

clean:
	rm -f $(OBJECTS) $(EXEC_NAME)
