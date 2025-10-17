#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include "token.h"
#include "dfa.h"

using namespace std;

enum LexerMode {
    DFA_MODE,
    SWITCH_MODE
};

class Lexer {
private:
    LexerMode mode;
    DFA dfa;
    map<string, Type> stateToTokenType;
    
    // Common helper methods
    bool isPascalKeyword(const string& word);
    bool isLogicalOperator(const string& word);
    bool isArithmeticOperator(const string& word);
    void skipWhitespace(FILE* file);
    
    // Switch-based lexer methods
    Token* readTokenSwitch(FILE* file);
    
    // DFA-based lexer methods
    void initializeStateMapping();
    Token* createToken(const string& state, const string& value);
    Token* readTokenDFA(FILE* file);
    
public:
    Lexer(LexerMode mode = DFA_MODE, const string& dfaRulesFile = "rules/pascal_lexicon.dfa");
    Token* readToken(FILE* file);
    vector<Token*> lex(FILE* file);
};

// Utility functions
FILE* read_file(const char* filename);
vector<Token*> lex_file(const char* filename, LexerMode mode = DFA_MODE);

#endif // LEXER_H