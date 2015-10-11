.PHONY: default
default: acl_simulator

CXX			= clang++
CC			= clang++
CXXFLAGS	= -g -Wall --std=c++11 $(INCLUDES)
LDFLAGS		= -g

acl_simulator: acl_simulator.o acl_entry.o file.o

acl_simulator.o: acl_simulator.cc acl_simulator.h acl_entry.h file.h

file.o: file.cc file.h acl_entry.h

acl_entry.o: acl_entry.cc acl_entry.h

.PHONY: clean
clean:
	rm -f *.o acl_simulator

.PHONY: all
all: clean default
