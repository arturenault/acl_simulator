.PHONY: default
default: acl_simulator

CXX			= clang++
CC			= clang++
CXXFLAGS	= -g -Wall $(INCLUDES)
LDFLAGS		= -g

acl_simulator: acl_simulator.o

acl_simulator.o: acl_simulator.cc

.PHONY: clean
clean:
	rm -f *.o acl_simulator

.PHONY: all
all: clean default
