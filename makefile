# Use g++ for C++ sources
CXX = g++
CXXFLAGS = -Wall -g -std=c++17

# IMPORTANT: -no-pie fixes the memory addresses so they don't randomize.
# This makes it possible to set breakpoints using hardcoded addresses.
TARGET_FLAGS = -no-pie

DBG = debugger
TARGET = target

DBG_SRC = debugger.cpp
TARGET_SRC = target.cpp

.PHONY: all run clean

all: $(DBG) $(TARGET)

# Build debugger (Standard build)
$(DBG): $(DBG_SRC)
	$(CXX) $(CXXFLAGS) -o $(DBG) $(DBG_SRC)

# Build target (Needs -no-pie to keep addresses static)
$(TARGET): $(TARGET_SRC)
	$(CXX) $(CXXFLAGS) $(TARGET_FLAGS) -o $(TARGET) $(TARGET_SRC)

# Run: Print the address of main, then start the debugger
run: all
	@echo "=========================================="
	@echo "SUGGESTED BREAKPOINT (Address of main):"
	@nm $(TARGET) | grep " T main" | awk '{print "0x" $$1}'
	@echo "=========================================="
	./$(DBG) ./$(TARGET)

clean:
	rm -f $(DBG) $(TARGET) *.o