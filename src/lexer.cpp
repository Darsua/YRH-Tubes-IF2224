#include <cstdio>
#include <vector>
#include <cctype>
#include <string>

#include "include/token.h"

using namespace std;

FILE* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    return file;
}

bool isPascalKeyword(const string& word) {
    return (word == "program" || word == "var" || word == "procedure" || 
            word == "begin" || word == "end" || word == "if" || word == "then" || 
            word == "else" || word == "while" || word == "do" || word == "for" ||
            word == "to" || word == "downto" || word == "integer" || word == "real" ||
            word == "boolean" || word == "char" || word == "array" || word == "of" ||
            word == "function" || word == "const" || word == "type");
}

bool isLogicalOperator(const string& word) {
    return (word == "and" || word == "or" || word == "not");
}

bool isArithmeticOperator(const string& word) {
    return (word == "div" || word == "mod");
}

Token* read_token(FILE* file) {
    int c = fgetc(file);
    
    switch(c) {
        case 'a'...'z':
        case 'A'...'Z': {
            string value(1, (char)c);
            int next_c;
            while ((next_c = fgetc(file)) && (isalnum(next_c) || next_c == '_')) {
                value += (char)next_c;
            }
            fseek(file, -1, SEEK_CUR);
            
            if (isPascalKeyword(value)) {
                return new Token(KEYWORD, value);
            } else if (isLogicalOperator(value)) {
                return new Token(LOGICAL_OPERATOR, value);
            } else if (isArithmeticOperator(value)) {
                return new Token(ARITHMETIC_OPERATOR, value);
            } else {
                return new Token(IDENTIFIER, value);
            }
        }
        
        case '0'...'9': {
            string value(1, (char)c);
            int next_c;
            while (isdigit(next_c = fgetc(file))) {
                value += (char)next_c;
            }
            fseek(file, -1, SEEK_CUR);
            return new Token(NUMBER, value);
            // TODO: Handle real numbers (with decimal points)
        }
        
        case '+':
        case '-':
        case '/':
            return new Token(ARITHMETIC_OPERATOR, string(1, (char)c));
            
        case '*': {
            int next_c = fgetc(file);
            if (next_c == ')') {
                return new Token(COMMENT_END, "*)");
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(ARITHMETIC_OPERATOR, "*");
            }
        }
            
        case '=':
            return new Token(RELATIONAL_OPERATOR, "=");
            
        case '<': {
            int next_c = fgetc(file);
            if (next_c == '>') {
                return new Token(RELATIONAL_OPERATOR, "<>");
            } else if (next_c == '=') {
                return new Token(RELATIONAL_OPERATOR, "<=");
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(RELATIONAL_OPERATOR, "<");
            }
        }
        
        case '>': {
            int next_c = fgetc(file);
            if (next_c == '=') {
                return new Token(RELATIONAL_OPERATOR, ">=");
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(RELATIONAL_OPERATOR, ">");
            }
        }
        
        case ':': {
            int next_c = fgetc(file);
            if (next_c == '=') {
                return new Token(ASSIGN_OPERATOR, ":=");
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(COLON, ":");
            }
        }
        
        case ';':
            return new Token(SEMICOLON, ";");
            
        case ',':
            return new Token(COMMA, ",");
            
        case '.': {
            int next_c = fgetc(file);
            if (next_c == '.') {
                return new Token(RANGE_OPERATOR, "..");
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(DOT, ".");
            }
        }
        
        case ')':
            return new Token(RPARENTHESIS, ")");
            
        case '[':
            return new Token(LBRACKET, "[");
            
        case ']':
            return new Token(RBRACKET, "]");
            
        case '{':
            return new Token(COMMENT_START, "{");
            // TODO: It might be better to ignore everything until the comment is closed
            
        case '}':
            return new Token(COMMENT_END, "}");
        
        case '\'': {
            // Pascal-S character and string literals use single quotes
            string value = "";
            int next_c;
            
            while ((next_c = fgetc(file)) != '\'' && next_c != EOF) {
                if (next_c == '\\') {
                    // Handle escape sequences (I don't know if we should)
                    int escaped = fgetc(file);
                    if (escaped != EOF) {
                        switch (escaped) {
                            case 'n': value += '\n'; break;
                            case 't': value += '\t'; break;
                            case 'r': value += '\r'; break;
                            case '\\': value += '\\'; break;
                            case '\'': value += '\''; break;
                            default: 
                                value += '\\';
                                value += (char)escaped;
                                break;
                        }
                    }
                } else {
                    value += (char)next_c;
                }
            }
            
            if (next_c == EOF) {
                printf("ERROR! Unterminated literal\n");
                exit(1);
            }
            
            // Determine if it's a character literal (single character) or string literal
            if (value.length() == 1) {
                return new Token(CHAR_LITERAL, value);
            } else {
                return new Token(STRING_LITERAL, value);
            }
        }
        
        case '(': {
            int next_c = fgetc(file);
            if (next_c == '*') {
                return new Token(COMMENT_START, "(*");
                // TODO: It might be better to ignore everything until the comment is closed
            } else {
                fseek(file, -1, SEEK_CUR);
                return new Token(LPARENTHESIS, "(");
            }
        }
    }
    
    return NULL;
}

vector<Token*> lex(FILE* file) {
    vector<Token*> tokens;
    Token* token;
    
    while (!feof(file)) {
        // Skip whitespace
        int c = fgetc(file);
        while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (feof(file)) break;
            c = fgetc(file);
        }
        if (feof(file)) break;
        
        fseek(file, -1, SEEK_CUR);
        token = read_token(file);
        
        if (token != NULL) {
            tokens.push_back(token);
            printf("%s\n", token->toString().c_str());
        } else {
            printf("ERROR! Unrecognized character: %c\n", c);
            exit(1);
        }
    }
    
    fclose(file);
    return tokens;
}