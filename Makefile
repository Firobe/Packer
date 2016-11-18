COMPILER = c++
CXXFLAGS = -Wall -Wfatal-errors -std=c++11 -isystem third_party -isystem third_party/boost/boost/ -O2
LIBS = -lboost_program_options
BOOST_LIBS=program_options,log
LIBS_DIR = -Lthird_party/boost/stage/lib/
EXEC_NAME = packer
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXAMPLE_FILE = group.svg
.PRECIOUS : %.o
.PHONY : clean
.PHONY : run

all: boost_libs $(OBJECTS)
	#make boost_libs must have been run once before
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

boost_libs:
	#Check if required libs are present on the machine
	#If not, extract boost if not already done
	#Then build boost if not already done 
	(P=true; \
	L=${BOOST_LIBS}; \
	IFS=','; for w in $$L; do \
		if [ ! $(locate -c "libboost_$$w") = 0 ]; then \
			echo "Not present : libboost_$$w"; \
			P=false; \
		fi; \
	done; \
	if [ $$P = false ] && [ ! -d "third_party/boost" ]; then \
		echo "Unpacking boost..."; \
		cd third_party/ && tar -xf boost.tar.bz2; \
		mv boost_1_62_0 boost; \
		cd ..; \
	fi; \
	if [ $$P = false ] && [ ! -d "third_party/boost/stage/lib" ]; then \
		cd third_party/boost && sh bootstrap.sh --with-libraries=$(BOOST_LIBS); \
		./b2; \
	fi)

install_plugin: all
	cp $(EXEC_NAME) ~/.config/inkscape/extensions/
	cp packer.inx ~/.config/inkscape/extensions/

clean:
	rm -f $(OBJECTS) $(EXEC_NAME) output.svg
