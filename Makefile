CXX = g++
CXXFLAGS = -Wall -Wextra

SRCDIR = src
BINDIR = bin

# Find all .cpp sources recursively under src/
SRCS := $(shell find $(SRCDIR) -type f -name '*.cpp')

# Include paths for headers
INCLUDES = -I. -Isrc -Iinclude -Isrc/include

# Target
TARGET = $(BINDIR)/compiler

all: $(TARGET)

# Formatting
CLANG_FORMAT = clang-format
FORMAT_SRCS = $(shell find . -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) -not -path './test/*')

.PHONY: format
format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo >&2 "clang-format not found. Install it or adjust Makefile."; exit 1; }
	$(CLANG_FORMAT) -i $(FORMAT_SRCS)
	@echo "Formatted $(FORMAT_SRCS)"

$(BINDIR):
	mkdir -p $(BINDIR)

# Main compiler
$(TARGET): $(SRCS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@

clean:
	rm -rf $(BINDIR)

rebuild: clean all

.PHONY: all clean rebuild format

