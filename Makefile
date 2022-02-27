# A simple Makefile for compiling small SDL projects

# Compiler
CC := clang

# Compiler flags
CFLAGS := -ggdb3 -O0 --std=c99 -Wall

# C preprocessor flags
CPPFLAGS := # Include path to header files here if required e.g. -I /usr/path/to/header/files

# Source files
SRCS := main.c

# Object file names
OBJS := $(SRCS:.c=.o)

# Linker flags
LDFLAGS := # Include path to libraries here if required e.g. -L /usr/path/to/libraries

# Libraries
LDLIBS := -lSDL2-2.0.0 -lSDL2_image-2.0.0

# Resources
RSRC := resources/

# Executable
EXEC := run

# Targets that should not be interpreted as file names
.PHONY: all clean

# Default recipe when no target is specified
all: $(EXEC)

# Recipe for building final executable
# Creates target executable with the name indicated to left of colon.
# If prerequisites on right of colon have been updated since last build, trigger a rebuild.
# Execute the tabbed section when invoked.
$(EXEC): $(OBJS) Makefile $(RSRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS) 

# Recipe for cleaning the workspace
clean:
	rm -f $(EXEC) $(OBJS)
