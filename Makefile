.PHONY: default
default: acl_sim

CXX			= clang++
CC			= clang++
CXXFLAGS	= -g -Wall $(INCLUDES)
LDFLAGS		= -g

acl_sim: acl_sim.o

acl_sim.o: acl_sim.cpp

.PHONY: clean
clean:
	rm -f *.o acl_sim

.PHONY: all
all: clean default
