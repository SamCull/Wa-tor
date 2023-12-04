# Makefile for SFML test program

# Compiler and flags
CXX = g++
CPPFLAGS = -Wall -I/home/sam/Documents/Wa-tor
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgomp

# Source files
SRCS = test.cpp 
OBJS = $(SRCS:.cpp=.o)
EXE = testApp

# Rule to build object files
%.o: %.cpp
	$(CXX) -c -o $@ $< $(CPPFLAGS) -std=c++11

# Main target
all: $(EXE)

# Rule to build the executable
$(EXE): $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) $(LDFLAGS)

# Clean up intermediate files
clean:
	rm -f $(OBJS)

# Clean up all generated files
distclean: clean
	rm -f $(EXE)

