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

# Formatting
CLANG_FORMAT = clang-format
FORMAT_SRCS = $(wildcard src/*.cpp) $(wildcard src/*.h) $(wildcard include/*.h)

.PHONY: format
format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo >&2 "clang-format not found. Install it or adjust Makefile."; exit 1; }
	$(CLANG_FORMAT) -i $(FORMAT_SRCS)
	@echo "Formatted $(FORMAT_SRCS)"

$(BINDIR):
	mkdir -p $(BINDIR)

# Main compiler
$(TARGET): $(CORE_SOURCES) $(MAIN_SOURCE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(CORE_SOURCES) $(MAIN_SOURCE) -o $@

clean:
	rm -rf $(BINDIR)

rebuild: clean all

.PHONY: all clean rebuild

