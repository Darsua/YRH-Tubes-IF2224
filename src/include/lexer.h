#ifndef LEXER_H
#define LEXER_H

#include <cstdio>
#include <vector>
#include "token.h"

using namespace std;

// Function to open and read a file
FILE* read_file(const char* filename);

// Function to read a single token from the file
Token* read_token(FILE* file);

// Function to tokenize the entire file and return a vector of tokens
vector<Token*> lex(FILE* file);

#endif // LEXER_H