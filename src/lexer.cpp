#include <cstdio>
#include <vector>
#include <cctype>
#include <string>
#include <map>
#include <utility>

#include "include/lexer.h"
#include "include/token.h"
#include "include/dfa.h"

using namespace std;

// Constructor
Lexer::Lexer(LexerMode mode, const string& dfaRulesFile) : mode(mode) {
    if (mode == DFA_MODE) {
        if (!dfa.loadDFAFromFile(dfaRulesFile)) {
            printf("ERROR: Failed to load DFA rules from %s\n", dfaRulesFile.c_str());
            printf("Falling back to switch-based lexer\n");
            this->mode = SWITCH_MODE;
        } else {
            initializeStateMapping();
        }
    }
}

// Common helper methods
bool Lexer::isPascalKeyword(const string& word) {
    return (word == "program" || word == "var" || word == "procedure" || 
            word == "begin" || word == "end" || word == "if" || word == "then" || 
            word == "else" || word == "while" || word == "do" || word == "for" ||
            word == "to" || word == "downto" || word == "integer" || word == "real" ||
            word == "boolean" || word == "char" || word == "array" || word == "of" ||
            word == "function" || word == "const" || word == "type");
}

bool Lexer::isLogicalOperator(const string& word) {
    return (word == "and" || word == "or" || word == "not");
}

bool Lexer::isArithmeticOperator(const string& word) {
    return (word == "div" || word == "mod");
}

void Lexer::skipWhitespace(FILE* file) {
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            ungetc(c, file);
            break;
        }
    }
}

// Switch-based token reading
Token* Lexer::readTokenSwitch(FILE* file) {
    int c = fgetc(file);
    
    if (c == EOF) {
        return nullptr;
    }
    
    switch(c) {
        case 'a'...'z':
        case 'A'...'Z': {
            string value(1, (char)c);
            int next_c;
            while ((next_c = fgetc(file)) != EOF && (isalnum(next_c) || next_c == '_')) {
                value += (char)next_c;
            }
            if (next_c != EOF) {
                ungetc(next_c, file);
            }
            
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
            
            // Handle integer part only - no decimal point handling
            while ((next_c = fgetc(file)) != EOF && isdigit(next_c)) {
                value += (char)next_c;
            }
            
            // Put back the non-digit character
            if (next_c != EOF) {
                ungetc(next_c, file);
            }
            
            return new Token(NUMBER, value);
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
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
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
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
                return new Token(RELATIONAL_OPERATOR, "<");
            }
        }
        
        case '>': {
            int next_c = fgetc(file);
            if (next_c == '=') {
                return new Token(RELATIONAL_OPERATOR, ">=");
            } else {
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
                return new Token(RELATIONAL_OPERATOR, ">");
            }
        }
        
        case ':': {
            int next_c = fgetc(file);
            if (next_c == '=') {
                return new Token(ASSIGN_OPERATOR, ":=");
            } else {
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
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
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
                return new Token(DOT, ".");
            }
        }
        
        case ')':
            return new Token(RPARENTHESIS, ")");
            
        case '[':
            return new Token(LBRACKET, "[");
            
        case ']':
            return new Token(RBRACKET, "]");
            
        case '{': {
            // Handle brace comments - read until closing brace
            string commentValue = "{";
            int next_c;
            while ((next_c = fgetc(file)) != EOF && next_c != '}') {
                commentValue += (char)next_c;
            }
            if (next_c == '}') {
                commentValue += '}';
            }
            return new Token(COMMENT_START, commentValue);
        }
        
        case '}':
            return new Token(COMMENT_END, "}");
        
        case '\'': {
            // Pascal string and character literals use single quotes
            string value = "";
            int next_c;
            
            while ((next_c = fgetc(file)) != '\'' && next_c != EOF) {
                if (next_c == '\\') {
                    // Handle escape sequences
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
            
            // Return string literal (both single char and multi-char are STRING_LITERAL to match DFA)
            return new Token(STRING_LITERAL, value);
        }
        
        case '(': {
            int next_c = fgetc(file);
            if (next_c == '*') {
                // Handle parenthesis comments - read until closing *)
                string commentValue = "(*";
                int prev_c = 0;
                while ((next_c = fgetc(file)) != EOF) {
                    commentValue += (char)next_c;
                    if (prev_c == '*' && next_c == ')') {
                        break;
                    }
                    prev_c = next_c;
                }
                return new Token(COMMENT_START, commentValue);
            } else {
                if (next_c != EOF) {
                    ungetc(next_c, file);
                }
                return new Token(LPARENTHESIS, "(");
            }
        }
        
        default:
            printf("ERROR! Unrecognized character: '%c' (ASCII %d)\n", (char)c, c);
            exit(1);
    }
    
    return nullptr;
}

// DFA-based lexer methods

// Initialize DFA state to token type mapping
void Lexer::initializeStateMapping() {
    stateToTokenType["S_ID"] = IDENTIFIER;
    stateToTokenType["S_NUM"] = NUMBER;

    stateToTokenType["S_CHAR_LITERAL"] = CHAR_LITERAL;
    stateToTokenType["S_STRING_LITERAL"] = STRING_LITERAL;
    stateToTokenType["S_STR_END"] = STRING_LITERAL;
    stateToTokenType["S_SEMICOLON"] = SEMICOLON;
    stateToTokenType["S_COMMA"] = COMMA;
    stateToTokenType["S_COLON_TEMP"] = COLON;
    stateToTokenType["S_DOT_TEMP"] = DOT;
    stateToTokenType["S_LPARENTHESIS"] = LPARENTHESIS;
    stateToTokenType["S_LPAREN_TEMP"] = LPARENTHESIS;
    stateToTokenType["S_RPARENTHESIS"] = RPARENTHESIS;
    stateToTokenType["S_LBRACKET"] = LBRACKET;
    stateToTokenType["S_RBRACKET"] = RBRACKET;
    stateToTokenType["S_PLUS"] = ARITHMETIC_OPERATOR;
    stateToTokenType["S_MINUS"] = ARITHMETIC_OPERATOR;
    stateToTokenType["S_MULTIPLY"] = ARITHMETIC_OPERATOR;
    stateToTokenType["S_DIVIDE"] = ARITHMETIC_OPERATOR;
    stateToTokenType["S_ASSIGN"] = ASSIGN_OPERATOR;
    stateToTokenType["S_EQ"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_NE"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_LT_TEMP"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_LE"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_GT_TEMP"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_GE"] = RELATIONAL_OPERATOR;
    stateToTokenType["S_RANGE"] = RANGE_OPERATOR;
    stateToTokenType["S_COMMENT_SINGLE"] = COMMENT_START;
    stateToTokenType["S_COMMENT_MULTI"] = COMMENT_START;
    stateToTokenType["S_COMMENT_BRACE"] = COMMENT_START;
    stateToTokenType["S_COMMENT_PAREN"] = COMMENT_START;
}

Token* Lexer::createToken(const string& state, const string& value) {
    auto it = stateToTokenType.find(state);
    if (it == stateToTokenType.end()) {
        printf("ERROR: Unknown DFA state: %s\n", state.c_str());
        exit(1);
    }
    
    Type tokenType = it->second;
    
    // Special handling for identifiers that might be keywords or operators
    if (tokenType == IDENTIFIER) {
        if (isPascalKeyword(value)) {
            tokenType = KEYWORD;
        } else if (isLogicalOperator(value)) {
            tokenType = LOGICAL_OPERATOR;
        } else if (isArithmeticOperator(value)) {
            tokenType = ARITHMETIC_OPERATOR;
        }
    }
    
    // Special handling for string literals - remove quotes from value
    if (tokenType == STRING_LITERAL || tokenType == CHAR_LITERAL) {
        if (value.length() >= 2 && value[0] == '\'' && value[value.length()-1] == '\'') {
            string content = value.substr(1, value.length() - 2);
            return new Token(tokenType, content);
        }
    }
    
    return new Token(tokenType, value);
}

// DFA-based token reading
Token* Lexer::readTokenDFA(FILE* file) {
    skipWhitespace(file);
    
    if (feof(file)) {
        return nullptr;
    }
    
    string currentState = dfa.getStartState();
    string tokenValue = "";
    string lastFinalState = "";
    string lastFinalValue = "";
    long lastFinalPosition = ftell(file);
    
    int c;
    while ((c = fgetc(file)) != EOF) {
        char input = (char)c;
        string nextState = dfa.getNextState(currentState, input);
        
        if (nextState == "ERROR") {
            // Can't continue, check if we have a valid token
            if (!lastFinalState.empty()) {
                // Backtrack to last final state position
                fseek(file, lastFinalPosition, SEEK_SET);
                return createToken(lastFinalState, lastFinalValue);
            } else {
                printf("ERROR: Unrecognized character '%c' at position %ld\n", input, ftell(file) - 1);
                exit(1);
            }
        }
        
        // Add character to token value
        tokenValue += input;
        currentState = nextState;
        
        // Check if current state is final
        if (dfa.isFinalState(currentState)) {
            lastFinalState = currentState;
            lastFinalValue = tokenValue;
            lastFinalPosition = ftell(file);
        }
    }
    
    // End of file reached
    if (dfa.isFinalState(currentState)) {
        return createToken(currentState, tokenValue);
    } else if (!lastFinalState.empty()) {
        fseek(file, lastFinalPosition, SEEK_SET);
        return createToken(lastFinalState, lastFinalValue);
    }
    
    return nullptr;
}

// Main token reading method - delegates to appropriate implementation
Token* Lexer::readToken(FILE* file) {
    if (mode == DFA_MODE) {
        return readTokenDFA(file);
    } else {
        return readTokenSwitch(file);
    }
}

// Main lexing method
vector<Token*> Lexer::lex(FILE* file) {
    vector<Token*> tokens;
    Token* token;
    
    // Skip initial whitespace for switch-based lexer
    if (mode == SWITCH_MODE) {
        skipWhitespace(file);
    }
    
    while (!feof(file)) {
        // For switch mode, skip whitespace between tokens
        if (mode == SWITCH_MODE) {
            skipWhitespace(file);
            if (feof(file)) break;
        }
        
        token = readToken(file);
        if (token != nullptr) {
            tokens.push_back(token);
            printf("%s\n", token->toString().c_str());
        } else if (mode == SWITCH_MODE && !feof(file)) {
            // Only report error if we're not at EOF
            int c = fgetc(file);
            if (c != EOF) {
                printf("ERROR! Unrecognized character: '%c'\n", (char)c);
                exit(1);
            }
        }
    }
    
    return tokens;
}

// Utility functions
FILE* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    return file;
}

vector<Token*> lex_file(const char* filename, LexerMode mode) {
    FILE* file = read_file(filename);
    if (file == NULL) {
        return vector<Token*>();
    }
    
    Lexer lexer(mode);
    vector<Token*> tokens = lexer.lex(file);
    fclose(file);
    return tokens;
}