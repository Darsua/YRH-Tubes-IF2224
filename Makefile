CXX = g++
CXXFLAGS = -Wall -Wextra

SRCDIR = src
BINDIR = bin

SOURCES = $(SRCDIR)/*.cpp

TARGET = $(BINDIR)/compiler

all: $(TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TARGET): $(SOURCES) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
	rm -rf $(BINDIR)

rebuild: clean all

.PHONY: all test clean rebuild
