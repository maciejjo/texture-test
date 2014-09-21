TARGET	= sokoban
CC			= clang
COMPILER = $(CC) -c
LINKER	= $(CC)
CFLAGS	= -Wall -Wextra -Wno-unused-parameter -pedantic -O2 -g -std=c11
LFLAGS	= -lkazmath -lglut -lGLEW -lGL -lm -lglfw -lassimp -lSOIL

SRCDIR	= src
OBJDIR	= obj
BINDIR	= bin
DEPDIR 	= dep

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS	:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

REMOVE 	:= rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(LFLAGS) -o $@ $(OBJECTS)
	@echo "Linking complete"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $< -o $@
	@echo "Compiled $<"

.PHONY: clean
clean:
	$(REMOVE) $(OBJECTS)
	@echo "Deleted $<"
