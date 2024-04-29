EXEC = kry
CXX = g++
CXXFLAGS = -Wall -Wextra -g -fsanitize=address -fsanitize=memory
SRCS = $(wildcard *.cpp)

.PHONY: all clean


all: $(EXEC)


$(EXEC): kry.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
	
clean:
	rm $(EXEC)

zip:	
	zip 241902.zip *.cpp Makefile README.md
