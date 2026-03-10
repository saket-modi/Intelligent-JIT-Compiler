# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall

# Targets
all: transpiler virtual_machine

transpiler: transpiler.cpp
	$(CXX) $(CXXFLAGS) -o transpiler transpiler.cpp

virtual_machine: virtual_machine.cpp
	$(CXX) $(CXXFLAGS) -o virtual_machine virtual_machine.cpp

# Clean up builds
clean:
	rm -f transpiler virtual_machine