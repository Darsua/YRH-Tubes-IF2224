#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum TYPE {
    KEYWORD,
    IDENTIFIER,
    ARITHMETIC_OPERATOR,
    RELATIONAL_OPERATOR,
    LOGICAL_OPERATOR,
    ASSIGN_OPERATOR,
    NUMBER,
    CHAR_LITERAL,
    STRING_LITERAL,
    SEMICOLON,
    COMMA,
    COLON,
    DOT,
    LPARENTHESIS,
    RPARENTHESIS,
    LBRACKET,
    RBRACKET,
    RANGE_OPERATOR,
    COMMENT_START,
    COMMENT_END
};

class Token {
private:
    TYPE type;
    string value;
    
public:
    Token(TYPE type, string value);
    string toString() const;
    TYPE getType() const;
    string getValue() const;
    string getTypeName() const;
};

#endif // TOKEN_H