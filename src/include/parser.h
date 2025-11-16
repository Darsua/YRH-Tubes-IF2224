#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include "token.h"

using namespace std;

class ParseNode {
private:
    string nodeType;
    string value;
    vector<shared_ptr<ParseNode>> children;
    
public:
    ParseNode(const string& type, const string& val = "");
    void addChild(shared_ptr<ParseNode> child);
    void print(int indent = 0) const;
    void printTree(const string& prefix = "", bool isLast = true) const;
    string getType() const { return nodeType; }
    string getValue() const { return value; }
    const vector<shared_ptr<ParseNode>>& getChildren() const { return children; }
};

class Parser {
private:
    vector<Token*> tokens;
    size_t currentPos;
    vector<string> errors;
    
    Token* currentToken();
    Token* peek(int offset = 1);
    void advance();
    bool match(Type type);
    bool matchKeyword(const string& keyword);
    void expect(Type type, const string& errorMsg);
    void expectKeyword(const string& keyword, const string& errorMsg);
    void syntaxError(const string& message);

    shared_ptr<ParseNode> parseProgram();
    shared_ptr<ParseNode> parseProgramHeader();
    shared_ptr<ParseNode> parseDeclarationPart();
    shared_ptr<ParseNode> parseConstDeclaration();
    shared_ptr<ParseNode> parseTypeDeclaration();
    shared_ptr<ParseNode> parseTypeDefinition();
    shared_ptr<ParseNode> parseVarDeclaration();
    shared_ptr<ParseNode> parseIdentifierList();
    shared_ptr<ParseNode> parseType();
    shared_ptr<ParseNode> parseArrayType();
    shared_ptr<ParseNode> parseRecordType();
    shared_ptr<ParseNode> parseRange();
    shared_ptr<ParseNode> parseSubprogramDeclaration();
    shared_ptr<ParseNode> parseProcedureDeclaration();
    shared_ptr<ParseNode> parseFunctionDeclaration();
    shared_ptr<ParseNode> parseFormalParameterList();
    shared_ptr<ParseNode> parseCompoundStatement();
    shared_ptr<ParseNode> parseStatementList();
    shared_ptr<ParseNode> parseAssignmentStatement();
    shared_ptr<ParseNode> parseIfStatement();
    shared_ptr<ParseNode> parseWhileStatement();
    shared_ptr<ParseNode> parseForStatement();
    shared_ptr<ParseNode> parseProcedureFunctionCall();
    shared_ptr<ParseNode> parseParameterList();
    shared_ptr<ParseNode> parseExpression();
    shared_ptr<ParseNode> parseSimpleExpression();
    shared_ptr<ParseNode> parseTerm();
    shared_ptr<ParseNode> parseFactor();
    shared_ptr<ParseNode> parseRelationalOperator();
    shared_ptr<ParseNode> parseAdditiveOperator();
    shared_ptr<ParseNode> parseMultiplicationOperator();
    
public:
    Parser(const vector<Token*>& tokenList);
    shared_ptr<ParseNode> parse();
    const vector<string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
};

#endif