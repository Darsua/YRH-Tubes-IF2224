#include "include/token.h"

using namespace std;

Token::Token(TYPE type, string value) : type(type), value(value) {}

string Token::toString() const {
    return getTypeName() + "(" + value + ")";
}

string Token::getValue() const {
    return value;
}

string Token::getTypeName() const {
    switch(type) {
        case KEYWORD: return "KEYWORD";
        case IDENTIFIER: return "IDENTIFIER";
        case ARITHMETIC_OPERATOR: return "ARITHMETIC_OPERATOR";
        case RELATIONAL_OPERATOR: return "RELATIONAL_OPERATOR";
        case LOGICAL_OPERATOR: return "LOGICAL_OPERATOR";
        case ASSIGN_OPERATOR: return "ASSIGN_OPERATOR";
        case NUMBER: return "NUMBER";
        case CHAR_LITERAL: return "CHAR_LITERAL";
        case STRING_LITERAL: return "STRING_LITERAL";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case DOT: return "DOT";
        case LPARENTHESIS: return "LPARENTHESIS";
        case RPARENTHESIS: return "RPARENTHESIS";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case RANGE_OPERATOR: return "RANGE_OPERATOR";
        case COMMENT_START: return "COMMENT_START";
        case COMMENT_END: return "COMMENT_END";
        default: return "UNKNOWN";
    }
}