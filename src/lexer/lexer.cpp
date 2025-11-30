#include <cstdio>
#include <vector>
#include <cctype>
#include <string>
#include <map>
#include <utility>

#include "../include/lexer.h"
#include "../include/token.h"
#include "../include/dfa.h"

using namespace std;

Lexer::Lexer(LexerMode mode, const string& dfaRulesFile) : mode(mode), currentLine(1) {
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

int Lexer::getChar(FILE* file) {
    int c = fgetc(file);
    if (c == '\n') {
        currentLine++;
    }
    return c;
}

void Lexer::ungetChar(int c, FILE* file) {
    if (c != EOF) {
        ungetc(c, file);
        if (c == '\n') {
            currentLine--;
        }
    }
}

bool Lexer::isPascalKeyword(const string& word) {
    return (word == "program" || word == "variabel" || word == "prosedur" || word == "mulai" ||
            word == "selesai" || word == "jika" || word == "maka" || word == "selain_itu" ||
            word == "selama" || word == "lakukan" || word == "untuk" || word == "ke" ||
            word == "turun_ke" || word == "integer" || word == "real" || word == "boolean" ||
            word == "char" || word == "larik" || word == "dari" || word == "fungsi" ||
            word == "konstanta" || word == "tipe" || word == "kasus" || word == "rekaman" ||
            word == "ulangi" || word == "sampai");
}

bool Lexer::isLogicalOperator(const string& word) {
    return (word == "dan" || word == "atau" || word == "tidak");
}

bool Lexer::isArithmeticOperator(const string& word) {
    return (word == "bagi" || word == "mod");
}

void Lexer::skipWhitespace(FILE* file) {
    int c;
    while ((c = getChar(file)) != EOF) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            ungetChar(c, file);
            break;
        }
    }
}

void Lexer::skipBraceComment(FILE* file) {
    int c;
    while ((c = getChar(file)) != EOF && c != '}') {
        // Do nothing, just keep consuming and tracking position
    }
}

// Helper method to skip parenthesis comments (* ... *)
void Lexer::skipParenComment(FILE* file) {
    int c;
    int prev_c = 0;
    while ((c = getChar(file)) != EOF) {
        if (prev_c == '*' && c == ')') {
            break;
        }
        prev_c = c;
    }
    // No need to put back - the closing *) is consumed
}

// Switch-based token reading
Token* Lexer::readTokenSwitch(FILE* file) {
    int tokenLine = currentLine;

    int c = getChar(file);

    if (c == EOF) {
        return nullptr;
    }

    switch (c) {
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_': {
            string value(1, (char)c);
            int next_c;
            while ((next_c = getChar(file)) != EOF && (isalnum(next_c) || next_c == '_')) {
                value += (char)next_c;
            }
            if (next_c != EOF) {
                ungetChar(next_c, file);
            }

            if (isPascalKeyword(value)) {
                return new Token(KEYWORD, value, tokenLine);
            } else if (isLogicalOperator(value)) {
                return new Token(LOGICAL_OPERATOR, value, tokenLine);
            } else if (isArithmeticOperator(value)) {
                return new Token(ARITHMETIC_OPERATOR, value, tokenLine);
            } else {
                return new Token(IDENTIFIER, value, tokenLine);
            }
        }

        case '0' ... '9': {
            string value(1, (char)c);
            int next_c;

            // Handle integer part only - no decimal point handling
            while ((next_c = getChar(file)) != EOF && isdigit(next_c)) {
                value += (char)next_c;
            }

            // Put back the non-digit character
            if (next_c != EOF) {
                ungetChar(next_c, file);
            }

            return new Token(NUMBER, value, tokenLine);
        }

        case '+':
        case '-':
        case '/':
            return new Token(ARITHMETIC_OPERATOR, string(1, (char)c), tokenLine);

        case '*':
            return new Token(ARITHMETIC_OPERATOR, "*", tokenLine);

        case '=':
            return new Token(RELATIONAL_OPERATOR, "=", tokenLine);

        case '<': {
            int next_c = getChar(file);
            if (next_c == '>') {
                return new Token(RELATIONAL_OPERATOR, "<>", tokenLine);
            } else if (next_c == '=') {
                return new Token(RELATIONAL_OPERATOR, "<=", tokenLine);
            } else {
                if (next_c != EOF) {
                    ungetChar(next_c, file);
                }
                return new Token(RELATIONAL_OPERATOR, "<", tokenLine);
            }
        }

        case '>': {
            int next_c = getChar(file);
            if (next_c == '=') {
                return new Token(RELATIONAL_OPERATOR, ">=", tokenLine);
            } else {
                if (next_c != EOF) {
                    ungetChar(next_c, file);
                }
                return new Token(RELATIONAL_OPERATOR, ">", tokenLine);
            }
        }

        case ':': {
            int next_c = getChar(file);
            if (next_c == '=') {
                return new Token(ASSIGN_OPERATOR, ":=", tokenLine);
            } else {
                if (next_c != EOF) {
                    ungetChar(next_c, file);
                }
                return new Token(COLON, ":", tokenLine);
            }
        }

        case ';':
            return new Token(SEMICOLON, ";", tokenLine);

        case ',':
            return new Token(COMMA, ",", tokenLine);

        case '.': {
            int next_c = getChar(file);
            if (next_c == '.') {
                return new Token(RANGE_OPERATOR, "..", tokenLine);
            } else {
                if (next_c != EOF) {
                    ungetChar(next_c, file);
                }
                return new Token(DOT, ".", tokenLine);
            }
        }

        case ')':
            return new Token(RPARENTHESIS, ")", tokenLine);

        case '[':
            return new Token(LBRACKET, "[", tokenLine);

        case ']':
            return new Token(RBRACKET, "]", tokenLine);

        case '{': {
            // Skip brace comments - read until closing brace
            skipBraceComment(file);
            // Skip whitespace after comment and continue to next token
            skipWhitespace(file);
            return readTokenSwitch(file);
        }

        case '}':
            printf("ERROR at line %d: Unexpected closing brace '}' - no matching opening brace\n",
                   tokenLine);
            exit(1);

        case '\'': {
            // Pascal string and character literals use single quotes
            string value = "";
            int next_c;

            while ((next_c = getChar(file)) != '\'' && next_c != EOF) {
                if (next_c == '\\') {
                    // Handle escape sequences
                    int escaped = getChar(file);
                    if (escaped != EOF) {
                        switch (escaped) {
                            case 'n':
                                value += '\n';
                                break;
                            case 't':
                                value += '\t';
                                break;
                            case 'r':
                                value += '\r';
                                break;
                            case '\\':
                                value += '\\';
                                break;
                            case '\'':
                                value += '\'';
                                break;
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
                printf("ERROR at line %d: Unterminated literal\n", tokenLine);
                exit(1);
            }

            // Distinguish between character literals and string literals to match DFA behavior
            // Empty ('') and single character ('a') → CHAR_LITERAL
            // Multi-character ('abc') → STRING_LITERAL
            if (value.length() <= 1) {
                return new Token(CHAR_LITERAL, value, tokenLine);
            } else {
                return new Token(STRING_LITERAL, value, tokenLine);
            }
        }

        case '(': {
            int next_c = getChar(file);
            if (next_c == '*') {
                // Skip parenthesis comments - read until closing *)
                skipParenComment(file);
                // Skip whitespace after comment and continue to next token
                skipWhitespace(file);
                return readTokenSwitch(file);
            } else {
                if (next_c != EOF) {
                    ungetChar(next_c, file);
                }
                return new Token(LPARENTHESIS, "(", tokenLine);
            }
        }

        default:
            printf("ERROR at line %d: Unrecognized character '%c'\n", tokenLine, (char)c);
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
    stateToTokenType["S_SINGLE_CHAR"] = CHAR_LITERAL;

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
}

Token* Lexer::createToken(const string& state, const string& value, int line) {
    // Check if this is a comment state - if so, return nullptr to indicate skip
    if (state == "S_COMMENT_SINGLE" || state == "S_COMMENT_MULTI") {
        return nullptr;  // Signal to skip this token
    }

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

    // Special handling for string literals - remove quotes and process escape sequences
    if (tokenType == STRING_LITERAL || tokenType == CHAR_LITERAL) {
        if (value.length() >= 2 && value[0] == '\'' && value[value.length() - 1] == '\'') {
            string raw_content = value.substr(1, value.length() - 2);
            string processed_content = "";

            // Process escape sequences in the content
            for (size_t i = 0; i < raw_content.length(); i++) {
                if (raw_content[i] == '\\' && i + 1 < raw_content.length()) {
                    char next_char = raw_content[i + 1];
                    switch (next_char) {
                        case 'n':
                            processed_content += '\n';
                            break;
                        case 't':
                            processed_content += '\t';
                            break;
                        case 'r':
                            processed_content += '\r';
                            break;
                        case '\\':
                            processed_content += '\\';
                            break;
                        case '\'':
                            processed_content += '\'';
                            break;
                        default:
                            processed_content += '\\';
                            processed_content += next_char;
                            break;
                    }
                    i++;  // Skip the next character as it's part of the escape sequence
                } else {
                    processed_content += raw_content[i];
                }
            }

            // Post-processing fix: Convert STRING_LITERAL to CHAR_LITERAL for single characters
            // This ensures DFA and switch lexers agree on single escaped characters like '\n'
            if (tokenType == STRING_LITERAL && processed_content.length() <= 1) {
                tokenType = CHAR_LITERAL;
            }

            return new Token(tokenType, processed_content, line);
        }
    }

    return new Token(tokenType, value, line);
}

// DFA-based token reading
Token* Lexer::readTokenDFA(FILE* file) {
    skipWhitespace(file);

    if (feof(file)) {
        return nullptr;
    }

    int tokenLine = currentLine;

    string currentState = dfa.getStartState();
    string tokenValue = "";
    string lastFinalState = "";
    string lastFinalValue = "";
    long lastFinalPosition = ftell(file);
    int lastFinalLine = tokenLine;

    int c;
    while ((c = getChar(file)) != EOF) {
        char input = (char)c;
        string nextState = dfa.getNextState(currentState, input);

        if (nextState == "ERROR") {
            // Can't continue, check if we have a valid token
            if (!lastFinalState.empty()) {
                // Backtrack to last final state position
                fseek(file, lastFinalPosition, SEEK_SET);
                currentLine = lastFinalLine;
                Token* token = createToken(lastFinalState, lastFinalValue, tokenLine);
                if (token == nullptr) {
                    // It was a comment, recursively get next token
                    return readTokenDFA(file);
                }
                return token;
            } else {
                printf("ERROR at line %d: Unrecognized character '%c'\n", currentLine, input);
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
            lastFinalLine = currentLine;
        }
    }

    // End of file reached
    if (dfa.isFinalState(currentState)) {
        Token* token = createToken(currentState, tokenValue, tokenLine);
        if (token == nullptr) {
            // It was a comment, recursively get next token
            return readTokenDFA(file);
        }
        return token;
    } else if (!lastFinalState.empty()) {
        fseek(file, lastFinalPosition, SEEK_SET);
        currentLine = lastFinalLine;
        Token* token = createToken(lastFinalState, lastFinalValue, tokenLine);
        if (token == nullptr) {
            // It was a comment, recursively get next token
            return readTokenDFA(file);
        }
        return token;
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
            // printf("%s\n", token->toString().c_str());
        } else if (mode == SWITCH_MODE && !feof(file)) {
            // Only report error if we're not at EOF
            int c = getChar(file);
            if (c != EOF) {
                printf("ERROR at line %d: Unrecognized character '%c'\n", currentLine, (char)c);
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