# NieR: Finite Automata

<p align="center"><img width="400" alt="image" src="https://github.com/user-attachments/assets/6ef29817-36db-4306-ac96-e9a1e56bdad8" /></p>
<p align="center"><i>Our YoRHa programmer, 2B. (by KornArt)</i></p>

A Pascal-S compiler built in C, implementing both lexical and syntax analysis. Written for IF2224 (Theory of Formal Languages and Automata) at STEI ITB.

---

## Team

| Name | NIM |
| :--- | :--- |
| Muhammad Ra'if Alkautsar | 13523011 |
| Fajar Kurniawan | 13523027 |
| Darrel Adinarya Sunanda | 13523061 |
| Reza Ahmad Syarif | 13523119 |

---

## Overview

The compiler processes Pascal-S source code through two stages:

1. **Lexical Analysis** — Converts raw source characters into tokens using a Deterministic Finite Automaton (DFA). Two implementations are available:
   - DFA-based lexer: rules loaded from an external `.dfa` file for flexibility
   - Switch-based lexer: hardcoded switch-case structure for performance

2. **Syntax Analysis** — Builds a parse tree from the token stream using a Recursive Descent Parser, verifying the structure of the program against the Pascal-S grammar.

---

## Requirements

- GCC
- Make

---

## Build

```bash
make all
```

Or for a clean rebuild:

```bash
make clean && make
```

The binary is output to `bin/compiler`.

---

## Usage

### Default (lexical + syntax analysis)
```bash
./bin/compiler <file.pas>
```

### Lexer only
```bash
./bin/compiler -l <file.pas>
./bin/compiler --lexer <file.pas>
```

### Switch-based lexer
```bash
./bin/compiler -s <file.pas>
```

### With execution timing
```bash
./bin/compiler -t <file.pas>
```

### Custom DFA rules file
```bash
./bin/compiler -d <rules_file.dfa> <file.pas>
```

### Parse tree output only
```bash
./bin/compiler --tree-only <file.pas>
```

### Combined options
```bash
./bin/compiler -s <file.pas>          # Switch-based lexer + parser
./bin/compiler -l -t <file.pas>       # Lexer only with timing
./bin/compiler --tree-only <file.pas> # Parse tree only
```

### Help
```bash
./bin/compiler -h
```

---

## Examples

```bash
# Default: lexical + syntax analysis
./bin/compiler test/milestone-1/base.pas

# Lexer only with timing
./bin/compiler -l -t test/milestone-1/hard.pas

# Switch-based lexer
./bin/compiler -s test/milestone-2/complete.pas

# Parse tree output only
./bin/compiler --tree-only test/milestone-2/dasar.pas
```

---

## License

MIT
