# MAKEFILE 7/4/2018
# HAPPY FOURTH OF JULY!

OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -w

LIBS = -lstdc++fs

OBJ_NAME = a

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LIBS) -o $(OBJ_NAME)
