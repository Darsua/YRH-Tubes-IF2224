CXX = g++
CXXFLAGS = -Wall -Wextra

SRCDIR = src
BINDIR = bin

# Core source files (exclude test files)
CORE_SOURCES = $(SRCDIR)/dfa.cpp $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp \
               $(SRCDIR)/token.cpp $(SRCDIR)/ast_nodes.cpp $(SRCDIR)/symbol_table.cpp \
               $(SRCDIR)/ast_builder.cpp $(SRCDIR)/semantic_analyzer.cpp

# Main compiler
MAIN_SOURCE = $(SRCDIR)/main.cpp

# Targets
TARGET = $(BINDIR)/compiler

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

# Main compiler
$(TARGET): $(CORE_SOURCES) $(MAIN_SOURCE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CORE_SOURCES) $(MAIN_SOURCE) -o $@

clean:
	rm -rf $(BINDIR)

rebuild: clean all

.PHONY: all clean rebuild
