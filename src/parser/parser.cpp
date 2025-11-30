#include "include/parser.h"
#include <iostream>
#include <iomanip>

using namespace std;

ParseNode::ParseNode(const string& type, const string& val, int ln)
    : nodeType(type), value(val), line(ln) {}

void ParseNode::addChild(shared_ptr<ParseNode> child) {
    if (child) {
        children.push_back(child);
    }
}

void ParseNode::print(int indent) const {
    printTree("", true);
}

void ParseNode::printTree(const string& prefix, bool isLast) const {
    bool isTerminal = true;
    for (char c : nodeType) {
        if (c != '(' && c != ')' && c != '_' && !isupper(c)) {
            isTerminal = false;
            break;
        }
    }

    if (isTerminal) {
        cout << nodeType;
    } else {
        cout << "<" << nodeType << ">";
    }

    if (!value.empty()) {
        cout << "(" << value << ")";
    }
    cout << endl;

    for (size_t i = 0; i < children.size(); i++) {
        bool isChildLast = (i == children.size() - 1);

        cout << prefix;

        if (isChildLast) {
            cout << "└── ";
        } else {
            cout << "├── ";
        }

        string newPrefix = prefix;
        if (isChildLast) {
            newPrefix += "    ";
        } else {
            newPrefix += "│   ";
        }
        children[i]->printTree(newPrefix, isChildLast);
    }
}

Parser::Parser(const vector<Token*>& tokenList) : tokens(tokenList), currentPos(0) {}

Token* Parser::currentToken() {
    if (currentPos < tokens.size()) {
        return tokens[currentPos];
    }
    return nullptr;
}

Token* Parser::peek(int offset) {
    size_t pos = currentPos + offset;
    if (pos < tokens.size()) {
        return tokens[pos];
    }
    return nullptr;
}

void Parser::advance() {
    if (currentPos < tokens.size()) {
        currentPos++;
    }
}

bool Parser::match(Type type) {
    Token* token = currentToken();
    return token && token->getType() == type;
}

bool Parser::matchKeyword(const string& keyword) {
    Token* token = currentToken();
    return token && token->getType() == KEYWORD && token->getValue() == keyword;
}

void Parser::expect(Type type, const string& errorMsg) {
    Token* token = currentToken();
    if (!token || token->getType() != type) {
        syntaxError(errorMsg);
        return;
    }
    advance();
}

void Parser::expectKeyword(const string& keyword, const string& errorMsg) {
    Token* token = currentToken();
    if (!token || token->getType() != KEYWORD || token->getValue() != keyword) {
        syntaxError(errorMsg);
        return;
    }
    advance();
}

void Parser::syntaxError(const string& message) {
    Token* token = currentToken();

    cerr << "\n========================================" << endl;
    cerr << "SYNTAX ERROR" << endl;
    cerr << "========================================" << endl;

    if (token) {
        cerr << "Location: Line " << token->getLine() << endl;
        cerr << "Position: Token #" << (currentPos + 1) << " of " << tokens.size() << endl;
        cerr << "Message:  " << message << endl;
        cerr << "Current:  " << token->toString() << endl;
    } else {
        cerr << "Location: <EOF> (Unexpected end of file)" << endl;
        cerr << "Position: Token #" << (currentPos + 1) << " of " << tokens.size() << endl;
        cerr << "Message:  " << message << endl;
    }

    if (currentPos > 0 && currentPos - 1 < tokens.size()) {
        cerr << "Previous: " << tokens[currentPos - 1]->toString() << endl;
    }
    if (currentPos + 1 < tokens.size()) {
        cerr << "Next:     " << tokens[currentPos + 1]->toString() << endl;
    }

    cerr << "========================================\n" << endl;

    string errorMsg = "Line " + to_string(token ? token->getLine() : 0) + ": " + message;
    errors.push_back(errorMsg);
}

shared_ptr<ParseNode> Parser::parse() {
    if (tokens.empty()) {
        syntaxError("No tokens to parse");
        return nullptr;
    }

    currentPos = 0;
    errors.clear();

    auto parseTree = parseProgram();

    if (currentToken() != nullptr && !hasErrors()) {
        syntaxError("Unexpected tokens after program end");
    }

    return parseTree;
}

// Grammar: <program> -> <program-header> + <declaration-part> + <compound-statement> + DOT
shared_ptr<ParseNode> Parser::parseProgram() {
    Token* startToken = currentToken();
    auto node = make_shared<ParseNode>("program", "", startToken ? startToken->getLine() : 0);

    auto header = parseProgramHeader();
    node->addChild(header);

    auto declarations = parseDeclarationPart();
    node->addChild(declarations);

    auto body = parseCompoundStatement();
    node->addChild(body);

    Token* dotToken = currentToken();
    expect(DOT, "Expected '.' at end of program");
    auto dotNode =
        make_shared<ParseNode>("DOT", dotToken->getValue(), dotToken ? dotToken->getLine() : 0);
    node->addChild(dotNode);

    return node;
}

// Grammar: <program-header> -> KEYWORD(program) + IDENTIFIER + SEMICOLON
shared_ptr<ParseNode> Parser::parseProgramHeader() {
    Token* progToken = currentToken();
    auto node = make_shared<ParseNode>("program-header", "", progToken ? progToken->getLine() : 0);

    expectKeyword("program", "Expected keyword 'program'");
    auto progNode = make_shared<ParseNode>("KEYWORD", progToken->getValue(),
                                           progToken ? progToken->getLine() : 0);
    node->addChild(progNode);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected program name (identifier)");
    } else {
        auto idNode =
            make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
        node->addChild(idNode);
        advance();
    }

    Token* semiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after program name");
    auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue(),
                                           semiToken ? semiToken->getLine() : 0);
    node->addChild(semiNode);

    return node;
}

// Grammar: <declaration-part> -> <const-declaration>* + <type-declaration>* + <var-declaration>* +
// <subprogram-declaration>*
shared_ptr<ParseNode> Parser::parseDeclarationPart() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("declaration-part", "", startToken ? startToken->getLine() : 0);

    if (matchKeyword("konstanta")) {
        auto constDecl = parseConstDeclaration();
        node->addChild(constDecl);
    }

    if (matchKeyword("tipe")) {
        auto typeDecl = parseTypeDeclaration();
        node->addChild(typeDecl);
    }

    if (matchKeyword("variabel")) {
        auto varDecl = parseVarDeclaration();
        node->addChild(varDecl);
    }

    while (matchKeyword("prosedur") || matchKeyword("fungsi")) {
        auto subprog = parseSubprogramDeclaration();
        node->addChild(subprog);
    }

    return node;
}

// Grammar: <const-declaration> -> KEYWORD(konstanta) + (IDENTIFIER RELATIONAL_OPERATOR(=)
// <constant-value> + SEMICOLON)+
shared_ptr<ParseNode> Parser::parseConstDeclaration() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("const-declaration", "", startToken ? startToken->getLine() : 0);

    Token* constToken = currentToken();
    expectKeyword("konstanta", "Expected 'konstanta'");
    auto constNode = make_shared<ParseNode>("KEYWORD", constToken->getValue(),
                                            constToken ? constToken->getLine() : 0);
    node->addChild(constNode);

    do {
        Token* token = currentToken();
        if (!match(IDENTIFIER)) {
            syntaxError("Expected constant name");
            break;
        }

        auto idNode =
            make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
        node->addChild(idNode);
        advance();

        Token* eqToken = currentToken();
        expect(RELATIONAL_OPERATOR, "Expected '=' in constant declaration");
        auto eqNode = make_shared<ParseNode>("RELATIONAL_OPERATOR", eqToken->getValue(),
                                             eqToken ? eqToken->getLine() : 0);
        node->addChild(eqNode);

        token = currentToken();
        token = currentToken();
        string constValue = "";

        if (match(ARITHMETIC_OPERATOR)) {
            string op = token->getValue();
            if (op == "+" || op == "-") {
                constValue = op;
                advance();
                token = currentToken();
            }
        }

        if (match(NUMBER) || match(STRING_LITERAL) || match(CHAR_LITERAL)) {
            constValue += token->getValue();
            advance();

            if (match(DOT)) {
                Token* nextToken = peek();
                if (nextToken && nextToken->getType() == NUMBER) {
                    advance();
                    constValue += ".";
                    constValue += currentToken()->getValue();
                    advance();
                }
            }
        } else {
            syntaxError("Expected constant value (number or string)");
        }

        auto valueNode =
            make_shared<ParseNode>("constant-value", constValue, token ? token->getLine() : 0);
        node->addChild(valueNode);

        Token* semiToken = currentToken();
        expect(SEMICOLON, "Expected ';' after constant declaration");
        auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue(),
                                               semiToken ? semiToken->getLine() : 0);
        node->addChild(semiNode);

    } while (match(IDENTIFIER) && !matchKeyword("tipe") && !matchKeyword("variabel") &&
             !matchKeyword("prosedur") && !matchKeyword("fungsi") && !matchKeyword("mulai"));

    return node;
}

// Grammar: <type-declaration> -> KEYWORD(tipe) + (<type-definition>)+
shared_ptr<ParseNode> Parser::parseTypeDeclaration() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("type-declaration", "", startToken ? startToken->getLine() : 0);

    Token* typeToken = currentToken();
    expectKeyword("tipe", "Expected 'tipe'");
    auto typeNode = make_shared<ParseNode>("KEYWORD", typeToken->getValue(),
                                           typeToken ? typeToken->getLine() : 0);
    node->addChild(typeNode);

    do {
        auto typeDef = parseTypeDefinition();
        node->addChild(typeDef);
    } while (match(IDENTIFIER) && !matchKeyword("variabel") && !matchKeyword("prosedur") &&
             !matchKeyword("fungsi") && !matchKeyword("mulai"));

    return node;
}

// Grammar: <type-definition> -> IDENTIFIER RELATIONAL_OPERATOR(=) <type> + SEMICOLON
shared_ptr<ParseNode> Parser::parseTypeDefinition() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("type-definition", "", startToken ? startToken->getLine() : 0);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected type name");
        return node;
    }

    auto idNode =
        make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
    node->addChild(idNode);
    advance();

    Token* eqToken = currentToken();
    expect(RELATIONAL_OPERATOR, "Expected '=' in type definition");
    auto eqNode = make_shared<ParseNode>("RELATIONAL_OPERATOR", eqToken->getValue(),
                                         eqToken ? eqToken->getLine() : 0);
    node->addChild(eqNode);

    auto typeNode = parseType();
    node->addChild(typeNode);

    Token* semiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after type definition");
    auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue(),
                                           semiToken ? semiToken->getLine() : 0);
    node->addChild(semiNode);

    return node;
}

// Grammar: <var-declaration> -> KEYWORD(variabel) (<identifier-list> + COLON + <type> + SEMICOLON)+
shared_ptr<ParseNode> Parser::parseVarDeclaration() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("var-declaration", "", startToken ? startToken->getLine() : 0);

    Token* varToken = currentToken();
    expectKeyword("variabel", "Expected 'variabel'");
    auto varNode =
        make_shared<ParseNode>("KEYWORD", varToken->getValue(), varToken ? varToken->getLine() : 0);
    node->addChild(varNode);

    do {
        auto idList = parseIdentifierList();
        node->addChild(idList);

        Token* colonToken = currentToken();
        expect(COLON, "Expected ':' after identifier list");
        auto colonNode = make_shared<ParseNode>("COLON", colonToken->getValue());
        node->addChild(colonNode);

        auto typeNode = parseType();
        node->addChild(typeNode);

        Token* semiToken = currentToken();
        expect(SEMICOLON, "Expected ';' after variable declaration");
        auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue());
        node->addChild(semiNode);

    } while (match(IDENTIFIER) && !matchKeyword("prosedur") && !matchKeyword("fungsi") &&
             !matchKeyword("mulai") && !matchKeyword("konstanta") && !matchKeyword("tipe"));

    return node;
}

// Grammar: <identifier-list> -> IDENTIFIER (COMMA + IDENTIFIER)*
shared_ptr<ParseNode> Parser::parseIdentifierList() {
    Token* startToken = currentToken();
    auto node =
        make_shared<ParseNode>("identifier-list", "", startToken ? startToken->getLine() : 0);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected identifier");
        return node;
    }

    auto idNode =
        make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
    node->addChild(idNode);
    advance();

    while (match(COMMA)) {
        Token* commaToken = currentToken();
        auto commaNode = make_shared<ParseNode>("COMMA", commaToken->getValue(),
                                                commaToken ? commaToken->getLine() : 0);
        node->addChild(commaNode);
        advance();

        token = currentToken();
        if (!match(IDENTIFIER)) {
            syntaxError("Expected identifier after ','");
            break;
        }

        idNode = make_shared<ParseNode>("IDENTIFIER", token->getValue(), token->getLine());
        node->addChild(idNode);
        advance();
    }

    return node;
}

// Grammar: <type> -> <array-type> | <record-type> | <range> | KEYWORD(integer|real|boolean|char) |
// IDENTIFIER
shared_ptr<ParseNode> Parser::parseType() {
    auto node = make_shared<ParseNode>("type", "", currentToken() ? currentToken()->getLine() : 0);

    if (matchKeyword("larik")) {
        auto arrayType = parseArrayType();
        node->addChild(arrayType);
    } else if (matchKeyword("rekaman")) {
        auto recordType = parseRecordType();
        node->addChild(recordType);
    } else if (match(NUMBER) || match(CHAR_LITERAL) || match(IDENTIFIER)) {
        Token* next = peek();
        if (next && next->getType() == RANGE_OPERATOR) {
            auto subrangeType = parseRange();
            node->addChild(subrangeType);
        } else if (match(NUMBER) || match(CHAR_LITERAL)) {
            syntaxError("Unexpected number or character literal in type declaration");
        } else {
            Token* token = currentToken();
            auto customType = make_shared<ParseNode>("custom-type", token->getValue());
            node->addChild(customType);
            advance();
        }
    } else {
        Token* token = currentToken();
        if (match(KEYWORD)) {
            string typeStr = token->getValue();
            if (typeStr == "integer" || typeStr == "real" || typeStr == "boolean" ||
                typeStr == "char") {
                auto simpleType = make_shared<ParseNode>("KEYWORD", typeStr);
                node->addChild(simpleType);
                advance();
            } else {
                syntaxError("Expected type (integer, real, boolean, char)");
            }
        } else {
            syntaxError("Expected type");
        }
    }

    return node;
}

// Grammar: <array-type> -> KEYWORD(larik) + LBRACKET + <range> + RBRACKET + KEYWORD(dari) + <type>
shared_ptr<ParseNode> Parser::parseArrayType() {
    auto node =
        make_shared<ParseNode>("array-type", "", currentToken() ? currentToken()->getLine() : 0);

    Token* larikToken = currentToken();
    expectKeyword("larik", "Expected 'larik'");
    auto larikNode = make_shared<ParseNode>("KEYWORD", larikToken->getValue());
    node->addChild(larikNode);

    if (match(LBRACKET)) {
        Token* lbracketToken = currentToken();
        auto lbracketNode = make_shared<ParseNode>("LBRACKET", lbracketToken->getValue());
        node->addChild(lbracketNode);
        advance();

        auto rangeNode = parseRange();
        node->addChild(rangeNode);

        Token* rbracketToken = currentToken();
        expect(RBRACKET, "Expected ']' after array range");
        auto rbracketNode = make_shared<ParseNode>("RBRACKET", rbracketToken->getValue());
        node->addChild(rbracketNode);
    }

    Token* dariToken = currentToken();
    expectKeyword("dari", "Expected 'dari' in array type");
    auto dariNode = make_shared<ParseNode>("KEYWORD", dariToken->getValue());
    node->addChild(dariNode);

    auto elementType = parseType();
    node->addChild(elementType);

    return node;
}

// Grammar: <record-type> -> KEYWORD(rekaman) + (<identifier-list> + COLON + <type> + SEMICOLON)+
// KEYWORD(selesai)
shared_ptr<ParseNode> Parser::parseRecordType() {
    auto node =
        make_shared<ParseNode>("record-type", "", currentToken() ? currentToken()->getLine() : 0);

    Token* rekamanToken = currentToken();
    expectKeyword("rekaman", "Expected 'rekaman'");
    auto rekamanNode = make_shared<ParseNode>("KEYWORD", rekamanToken->getValue());
    node->addChild(rekamanNode);

    while (!matchKeyword("selesai") && currentToken()) {
        auto fieldNode = make_shared<ParseNode>("field-declaration", "",
                                                currentToken() ? currentToken()->getLine() : 0);

        auto idList = parseIdentifierList();
        fieldNode->addChild(idList);

        Token* colonToken = currentToken();
        expect(COLON, "Expected ':' after field names");
        auto colonNode = make_shared<ParseNode>("COLON", colonToken->getValue(),
                                                colonToken ? colonToken->getLine() : 0);
        fieldNode->addChild(colonNode);

        auto typeNode = parseType();
        fieldNode->addChild(typeNode);

        Token* semiToken = currentToken();
        expect(SEMICOLON, "Expected ';' after field declaration");
        auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue(),
                                               semiToken ? semiToken->getLine() : 0);
        fieldNode->addChild(semiNode);

        node->addChild(fieldNode);
    }

    Token* selesaiToken = currentToken();
    expectKeyword("selesai", "Expected 'selesai' to end record");
    auto selesaiNode = make_shared<ParseNode>("KEYWORD", selesaiToken->getValue(),
                                              selesaiToken ? selesaiToken->getLine() : 0);
    node->addChild(selesaiNode);

    return node;
}

// Grammar: <range> -> <expression> + RANGE_OPERATOR(..) + <expression>
shared_ptr<ParseNode> Parser::parseRange() {
    auto node = make_shared<ParseNode>("range", "", currentToken() ? currentToken()->getLine() : 0);

    auto lowerExpr = parseExpression();
    node->addChild(lowerExpr);

    Token* rangeToken = currentToken();
    expect(RANGE_OPERATOR, "Expected '..' in range");
    auto rangeOpNode = make_shared<ParseNode>("RANGE_OPERATOR", rangeToken->getValue());
    node->addChild(rangeOpNode);

    auto upperExpr = parseExpression();
    node->addChild(upperExpr);

    return node;
}

// Grammar: <subprogram-declaration> -> <procedure-declaration> | <function-declaration>
shared_ptr<ParseNode> Parser::parseSubprogramDeclaration() {
    auto node = make_shared<ParseNode>("subprogram-declaration");

    if (matchKeyword("prosedur")) {
        auto procDecl = parseProcedureDeclaration();
        node->addChild(procDecl);
    } else if (matchKeyword("fungsi")) {
        auto funcDecl = parseFunctionDeclaration();
        node->addChild(funcDecl);
    } else {
        syntaxError("Expected 'prosedur' or 'fungsi'");
    }

    return node;
}

// Grammar: <procedure-declaration> -> KEYWORD(prosedur) + IDENTIFIER + (<formal-parameter-list>)? +
// SEMICOLON + <declaration-part> + <compound-statement> + SEMICOLON
shared_ptr<ParseNode> Parser::parseProcedureDeclaration() {
    auto node = make_shared<ParseNode>("procedure-declaration");

    Token* prosedurToken = currentToken();
    expectKeyword("prosedur", "Expected 'prosedur'");
    auto prosedurNode = make_shared<ParseNode>("KEYWORD", prosedurToken->getValue());
    node->addChild(prosedurNode);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected procedure name");
    } else {
        auto idNode =
            make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
        node->addChild(idNode);
        advance();
    }

    if (match(LPARENTHESIS)) {
        auto params = parseFormalParameterList();
        node->addChild(params);
    }

    Token* semiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after procedure header");
    auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue());
    node->addChild(semiNode);

    auto declarations = parseDeclarationPart();
    node->addChild(declarations);

    auto body = parseCompoundStatement();
    node->addChild(body);

    Token* endSemiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after procedure body");
    auto endSemiNode = make_shared<ParseNode>("SEMICOLON", endSemiToken->getValue());
    node->addChild(endSemiNode);

    return node;
}

// Grammar: <function-declaration> -> KEYWORD(fungsi) + IDENTIFIER + (<formal-parameter-list>)? +
// COLON + <type> + SEMICOLON + <declaration-part> + <compound-statement> + SEMICOLON
shared_ptr<ParseNode> Parser::parseFunctionDeclaration() {
    auto node = make_shared<ParseNode>("function-declaration");

    Token* fungsiToken = currentToken();
    expectKeyword("fungsi", "Expected 'fungsi'");
    auto fungsiNode = make_shared<ParseNode>("KEYWORD", fungsiToken->getValue());
    node->addChild(fungsiNode);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected function name");
    } else {
        auto idNode =
            make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
        node->addChild(idNode);
        advance();
    }

    if (match(LPARENTHESIS)) {
        auto params = parseFormalParameterList();
        node->addChild(params);
    }

    Token* colonToken = currentToken();
    expect(COLON, "Expected ':' before return type");
    auto colonNode = make_shared<ParseNode>("COLON", colonToken->getValue(),
                                            colonToken ? colonToken->getLine() : 0);
    node->addChild(colonNode);

    auto returnType = parseType();
    node->addChild(returnType);

    Token* semiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after function header");
    auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue());
    node->addChild(semiNode);

    auto declarations = parseDeclarationPart();
    node->addChild(declarations);

    auto body = parseCompoundStatement();
    node->addChild(body);

    Token* endSemiToken = currentToken();
    expect(SEMICOLON, "Expected ';' after function body");
    auto endSemiNode = make_shared<ParseNode>("SEMICOLON", endSemiToken->getValue());
    node->addChild(endSemiNode);

    return node;
}

// Grammar: <formal-parameter-list> -> LPARENTHESIS + ((KEYWORD(variabel))? + <identifier-list> + COLON + <type> +
// (SEMICOLON + (KEYWORD(variabel))? + <identifier-list> + COLON + <type>)*)? + RPARENTHESIS
shared_ptr<ParseNode> Parser::parseFormalParameterList() {
    auto node = make_shared<ParseNode>("formal-parameter-list");

    Token* lparenToken = currentToken();
    expect(LPARENTHESIS, "Expected '(' in parameter list");
    auto lparenNode = make_shared<ParseNode>("LPARENTHESIS", lparenToken->getValue());
    node->addChild(lparenNode);

    if (!match(RPARENTHESIS)) {
        do {
            if (matchKeyword("variabel")) {
                Token* varToken = currentToken();
                auto varNode = make_shared<ParseNode>("KEYWORD", varToken->getValue());
                node->addChild(varNode);
                advance();
            }

            auto idList = parseIdentifierList();
            node->addChild(idList);

            Token* colonToken = currentToken();
            expect(COLON, "Expected ':' after parameter names");
            auto colonNode = make_shared<ParseNode>("COLON", colonToken->getValue());
            node->addChild(colonNode);

            auto typeNode = parseType();
            node->addChild(typeNode);

            if (match(SEMICOLON)) {
                Token* semiToken = currentToken();
                auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue());
                node->addChild(semiNode);
                advance();
            } else {
                break;
            }
        } while (!match(RPARENTHESIS));
    }

    Token* rparenToken = currentToken();
    expect(RPARENTHESIS, "Expected ')' after parameter list");
    auto rparenNode = make_shared<ParseNode>("RPARENTHESIS", rparenToken->getValue());
    node->addChild(rparenNode);

    return node;
}

// Grammar: <compound-statement> -> KEYWORD(mulai) + <statement-list> + KEYWORD(selesai)
shared_ptr<ParseNode> Parser::parseCompoundStatement() {
    auto node = make_shared<ParseNode>("compound-statement");

    Token* mulaiToken = currentToken();
    expectKeyword("mulai", "Expected 'mulai'");
    auto mulaiNode = make_shared<ParseNode>("KEYWORD", mulaiToken->getValue());
    node->addChild(mulaiNode);

    auto stmtList = parseStatementList();
    node->addChild(stmtList);

    Token* selesaiToken = currentToken();
    expectKeyword("selesai", "Expected 'selesai'");
    auto selesaiNode = make_shared<ParseNode>("KEYWORD", selesaiToken->getValue());
    node->addChild(selesaiNode);

    return node;
}

// Grammar: <statement-list> -> <assignment-statement> | <if-statement> | <while-statement> |
// <for-statement> | <procedure-call> | <compound-statement> + (SEMICOLON + <assignment-statement> |
// <if-statement> | <while-statement> | <for-statement> | <procedure-call> | <compound-statement>)*
shared_ptr<ParseNode> Parser::parseStatementList() {
    auto node = make_shared<ParseNode>("statement-list");

    while (!matchKeyword("selesai") && currentToken()) {
        bool isProcedureCall = false;

        if (matchKeyword("jika")) {
            auto stmt = parseIfStatement();
            node->addChild(stmt);
        } else if (matchKeyword("selama")) {
            auto stmt = parseWhileStatement();
            node->addChild(stmt);
        } else if (matchKeyword("untuk")) {
            auto stmt = parseForStatement();
            node->addChild(stmt);
        } else if (matchKeyword("mulai")) {
            auto stmt = parseCompoundStatement();
            node->addChild(stmt);
        } else if (match(IDENTIFIER)) {
            size_t lookahead = currentPos + 1;
            bool foundAssign = false;

            while (lookahead < tokens.size() && tokens[lookahead]->getType() == DOT) {
                lookahead++;  // skip DOT

                if (lookahead < tokens.size() && tokens[lookahead]->getType() == DOT) {
                    break;  // It's a range operator, not field access
                }

                if (lookahead < tokens.size() && tokens[lookahead]->getType() == IDENTIFIER) {
                    lookahead++;
                }
            }

            while (lookahead < tokens.size() && tokens[lookahead]->getType() == LBRACKET) {
                lookahead++;  // skip LBRACKET

                int bracketDepth = 1;
                while (lookahead < tokens.size() && bracketDepth > 0) {
                    if (tokens[lookahead]->getType() == LBRACKET) bracketDepth++;
                    if (tokens[lookahead]->getType() == RBRACKET) bracketDepth--;
                    lookahead++;
                }
            }

            if (lookahead < tokens.size() && tokens[lookahead]->getType() == ASSIGN_OPERATOR) {
                foundAssign = true;
            }

            if (foundAssign) {
                auto stmt = parseAssignmentStatement();
                node->addChild(stmt);
            } else {
                auto stmt = parseProcedureFunctionCall();
                node->addChild(stmt);
                isProcedureCall = true;
            }
        } else {
            break;
        }

        if (!matchKeyword("selesai") && currentToken()) {
            if (!isProcedureCall) {
                Token* semiToken = currentToken();
                expect(SEMICOLON, "Expected ';' after statement");
                if (semiToken && semiToken->getType() == SEMICOLON) {
                    auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue());
                    node->addChild(semiNode);
                }
            }
        }
    }

    return node;
}

// Grammar: <assignment-statement> -> IDENTIFIER (DOT IDENTIFIER | LBRACKET <expression> RBRACKET)*
// ASSIGN_OPERATOR <expression>
shared_ptr<ParseNode> Parser::parseAssignmentStatement() {
    auto node = make_shared<ParseNode>("assignment-statement");

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected identifier in assignment");
        return node;
    }

    auto idNode = make_shared<ParseNode>("IDENTIFIER", token->getValue(), token->getLine());
    node->addChild(idNode);
    advance();

    while (match(DOT)) {
        Token* nextToken = peek();
        if (nextToken && nextToken->getType() == DOT) {
            break;
        }

        Token* dotToken = currentToken();
        auto dotNode = make_shared<ParseNode>("DOT", dotToken->getValue(), dotToken->getLine());
        node->addChild(dotNode);
        advance();

        token = currentToken();
        if (!match(IDENTIFIER)) {
            syntaxError("Expected field name after '.'");
            break;
        }

        auto fieldNode = make_shared<ParseNode>("IDENTIFIER", token->getValue(), token->getLine());
        node->addChild(fieldNode);
        advance();
    }

    while (match(LBRACKET)) {
        advance();
        auto indexNode = make_shared<ParseNode>("array-index");
        auto indexExpr = parseExpression();
        indexNode->addChild(indexExpr);
        node->addChild(indexNode);
        expect(RBRACKET, "Expected ']' after array index");
    }

    Token* assignToken = currentToken();
    expect(ASSIGN_OPERATOR, "Expected ':=' in assignment");
    auto assignNode = make_shared<ParseNode>("ASSIGN_OPERATOR", assignToken->getValue());
    node->addChild(assignNode);

    auto exprNode = parseExpression();
    node->addChild(exprNode);

    return node;
}

// Grammar: <if-statement> -> KEYWORD(jika) + <expression> + KEYWORD(maka) + <assignment-statement>
// | <procedure-call> | <compound-statement> + (KEYWORD(selain_itu) + <assignment-statement> |
// <procedure-call> | <compound-statement>)?
shared_ptr<ParseNode> Parser::parseIfStatement() {
    auto node = make_shared<ParseNode>("if-statement");

    Token* jikaToken = currentToken();
    expectKeyword("jika", "Expected 'jika'");
    auto jikaNode = make_shared<ParseNode>("KEYWORD", jikaToken->getValue());
    node->addChild(jikaNode);

    auto condition = parseExpression();
    node->addChild(condition);

    Token* makaToken = currentToken();
    expectKeyword("maka", "Expected 'maka'");
    auto makaNode = make_shared<ParseNode>("KEYWORD", makaToken->getValue());
    node->addChild(makaNode);

    if (matchKeyword("mulai")) {
        auto thenStmt = parseCompoundStatement();
        node->addChild(thenStmt);
    } else {
        auto thenStmt = make_shared<ParseNode>("then-statement");
        if (match(IDENTIFIER)) {
            Token* next = peek();
            if (next && next->getType() == ASSIGN_OPERATOR) {
                auto stmt = parseAssignmentStatement();
                thenStmt->addChild(stmt);
            } else {
                auto stmt = parseProcedureFunctionCall();
                thenStmt->addChild(stmt);
            }
        }
        node->addChild(thenStmt);
    }

    if (matchKeyword("selain_itu")) {
        Token* selainToken = currentToken();
        advance();
        auto selainNode = make_shared<ParseNode>("KEYWORD", selainToken->getValue());
        node->addChild(selainNode);

        if (matchKeyword("mulai")) {
            auto elseStmt = parseCompoundStatement();
            node->addChild(elseStmt);
        } else {
            auto elseStmt = make_shared<ParseNode>("else-statement");
            if (match(IDENTIFIER)) {
                Token* next = peek();
                if (next && next->getType() == ASSIGN_OPERATOR) {
                    auto stmt = parseAssignmentStatement();
                    elseStmt->addChild(stmt);
                } else {
                    auto stmt = parseProcedureFunctionCall();
                    elseStmt->addChild(stmt);
                }
            }
            node->addChild(elseStmt);
        }
    }

    return node;
}

// Grammar: <while-statement> -> KEYWORD(selama) + <expression> + KEYWORD(lakukan) +
// (<compound-statement> | <assignment-statement> | <procedure-call>)
shared_ptr<ParseNode> Parser::parseWhileStatement() {
    auto node = make_shared<ParseNode>("while-statement");

    Token* selamaToken = currentToken();
    expectKeyword("selama", "Expected 'selama'");
    auto selamaNode = make_shared<ParseNode>("KEYWORD", selamaToken->getValue());
    node->addChild(selamaNode);

    auto condition = parseExpression();
    node->addChild(condition);

    Token* lakukanToken = currentToken();
    expectKeyword("lakukan", "Expected 'lakukan'");
    auto lakukanNode = make_shared<ParseNode>("KEYWORD", lakukanToken->getValue());
    node->addChild(lakukanNode);

    if (matchKeyword("mulai")) {
        auto body = parseCompoundStatement();
        node->addChild(body);
    } else {
        auto bodyNode = make_shared<ParseNode>("statement");
        if (match(IDENTIFIER)) {
            size_t lookahead = currentPos + 1;
            bool foundAssign = false;

            while (lookahead < tokens.size() && tokens[lookahead]->getType() == LBRACKET) {
                lookahead++;
                int bracketDepth = 1;
                while (lookahead < tokens.size() && bracketDepth > 0) {
                    if (tokens[lookahead]->getType() == LBRACKET) bracketDepth++;
                    if (tokens[lookahead]->getType() == RBRACKET) bracketDepth--;
                    lookahead++;
                }
            }

            if (lookahead < tokens.size() && tokens[lookahead]->getType() == ASSIGN_OPERATOR) {
                foundAssign = true;
            }

            if (foundAssign) {
                auto stmt = parseAssignmentStatement();
                bodyNode->addChild(stmt);
            } else {
                auto stmt = parseProcedureFunctionCall();
                bodyNode->addChild(stmt);
            }
        }
        node->addChild(bodyNode);
    }

    return node;
}

// Grammar: <for-statement> -> KEYWORD(untuk) + IDENTIFIER ASSIGN_OPERATOR <expression> +
// (KEYWORD(ke) | KEYWORD(turun_ke)) + <expression>  + KEYWORD(lakukan) + <compound-statement> |
// <assignment-statement> | <procedure-call>
shared_ptr<ParseNode> Parser::parseForStatement() {
    auto node = make_shared<ParseNode>("for-statement");

    Token* untukToken = currentToken();
    expectKeyword("untuk", "Expected 'untuk'");
    auto untukNode = make_shared<ParseNode>("KEYWORD", untukToken->getValue());
    node->addChild(untukNode);

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected loop variable");
        return node;
    }

    auto varNode =
        make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
    node->addChild(varNode);
    advance();

    Token* assignToken = currentToken();
    expect(ASSIGN_OPERATOR, "Expected ':=' in for loop");
    auto assignNode = make_shared<ParseNode>("ASSIGN_OPERATOR", assignToken->getValue());
    node->addChild(assignNode);

    auto startExpr = parseExpression();
    node->addChild(startExpr);

    if (matchKeyword("ke")) {
        Token* keToken = currentToken();
        auto dirNode = make_shared<ParseNode>("KEYWORD", keToken->getValue());
        node->addChild(dirNode);
        advance();
    } else if (matchKeyword("turun_ke")) {
        Token* turunToken = currentToken();
        auto dirNode = make_shared<ParseNode>("KEYWORD", turunToken->getValue());
        node->addChild(dirNode);
        advance();
    } else {
        syntaxError("Expected 'ke' or 'turun_ke'");
    }

    auto endExpr = parseExpression();
    node->addChild(endExpr);

    Token* lakukanToken = currentToken();
    expectKeyword("lakukan", "Expected 'lakukan'");
    auto lakukanNode = make_shared<ParseNode>("KEYWORD", lakukanToken->getValue());
    node->addChild(lakukanNode);

    if (matchKeyword("mulai")) {
        auto body = parseCompoundStatement();
        node->addChild(body);
    } else {
        auto bodyNode = make_shared<ParseNode>("statement");
        if (match(IDENTIFIER)) {
            size_t lookahead = currentPos + 1;
            bool foundAssign = false;

            while (lookahead < tokens.size() && tokens[lookahead]->getType() == LBRACKET) {
                lookahead++;
                int bracketDepth = 1;
                while (lookahead < tokens.size() && bracketDepth > 0) {
                    if (tokens[lookahead]->getType() == LBRACKET) bracketDepth++;
                    if (tokens[lookahead]->getType() == RBRACKET) bracketDepth--;
                    lookahead++;
                }
            }

            if (lookahead < tokens.size() && tokens[lookahead]->getType() == ASSIGN_OPERATOR) {
                foundAssign = true;
            }

            if (foundAssign) {
                auto stmt = parseAssignmentStatement();
                bodyNode->addChild(stmt);
            } else {
                auto stmt = parseProcedureFunctionCall();
                bodyNode->addChild(stmt);
            }
        }
        node->addChild(bodyNode);
    }

    return node;
}

// Grammar: <procedure/function-call> -> IDENTIFIER + (LPARENTHESIS + <parameter-list> +
// RPARENTHESIS)? + (SEMICOLON)?
shared_ptr<ParseNode> Parser::parseProcedureFunctionCall() {
    auto node = make_shared<ParseNode>("procedure-call");

    Token* token = currentToken();
    if (!match(IDENTIFIER)) {
        syntaxError("Expected procedure/function name");
        return node;
    }

    string procName = token->getValue();
    auto idNode = make_shared<ParseNode>("IDENTIFIER", procName, token ? token->getLine() : 0);
    node->addChild(idNode);
    advance();

    if (match(LPARENTHESIS)) {
        Token* lparenToken = currentToken();
        auto lparenNode = make_shared<ParseNode>("LPARENTHESIS", lparenToken->getValue());
        node->addChild(lparenNode);

        auto params = parseParameterList();
        node->addChild(params);

        Token* rparenToken = tokens[currentPos - 1];
        auto rparenNode = make_shared<ParseNode>("RPARENTHESIS", rparenToken->getValue());
        node->addChild(rparenNode);
    }

    if (match(SEMICOLON)) {
        Token* semiToken = currentToken();
        auto semiNode = make_shared<ParseNode>("SEMICOLON", semiToken->getValue(),
                                               semiToken ? semiToken->getLine() : 0);
        node->addChild(semiNode);
        advance();
    }

    return node;
}

// Grammar: <parameter-list> -> LPARENTHESIS + (<expression> + (COMMA <expression>)*)? +
// RPARENTHESIS
shared_ptr<ParseNode> Parser::parseParameterList() {
    auto node = make_shared<ParseNode>("parameter-list");

    expect(LPARENTHESIS, "Expected '(' in parameter list");

    if (!match(RPARENTHESIS)) {
        auto expr = parseExpression();
        node->addChild(expr);

        while (match(COMMA)) {
            Token* commaToken = currentToken();
            auto commaNode = make_shared<ParseNode>("COMMA", commaToken->getValue());
            node->addChild(commaNode);
            advance();

            expr = parseExpression();
            node->addChild(expr);
        }
    }

    expect(RPARENTHESIS, "Expected ')' after parameter list");

    return node;
}

// Grammar: <expression> -> <simple-expression> + (RELATIONAL_OPERATOR + <simple-expression>)?
shared_ptr<ParseNode> Parser::parseExpression() {
    auto node = make_shared<ParseNode>("expression");

    auto left = parseSimpleExpression();
    node->addChild(left);

    if (match(RELATIONAL_OPERATOR)) {
        auto opNode = parseRelationalOperator();
        node->addChild(opNode);

        auto right = parseSimpleExpression();
        node->addChild(right);
    }

    return node;
}

// Grammar: <simple-expression> -> (ARITHMETIC_OPERATOR(+|-))? <term> (<additive-operator> <term>)*
shared_ptr<ParseNode> Parser::parseSimpleExpression() {
    auto node = make_shared<ParseNode>("simple-expression");

    if (match(ARITHMETIC_OPERATOR)) {
        Token* token = currentToken();
        string op = token->getValue();
        if (op == "+" || op == "-") {
            auto unaryNode = make_shared<ParseNode>("ARITHMETIC_OPERATOR", op);
            node->addChild(unaryNode);
            advance();
        }
    }

    auto term = parseTerm();
    node->addChild(term);

    while (match(ARITHMETIC_OPERATOR) || match(LOGICAL_OPERATOR)) {
        Token* token = currentToken();
        string op = token->getValue();

        if (op == "+" || op == "-" || op == "atau") {
            auto opNode = parseAdditiveOperator();
            node->addChild(opNode);

            term = parseTerm();
            node->addChild(term);
        } else {
            break;
        }
    }

    return node;
}

// Grammar: <term> -> <factor> (<multiplicative-operator> <factor>)*
shared_ptr<ParseNode> Parser::parseTerm() {
    auto node = make_shared<ParseNode>("term");

    auto factor = parseFactor();
    node->addChild(factor);

    while (match(ARITHMETIC_OPERATOR) || match(LOGICAL_OPERATOR)) {
        Token* token = currentToken();
        string op = token->getValue();

        if (op == "*" || op == "/" || op == "bagi" || op == "mod" || op == "dan") {
            auto opNode = parseMultiplicationOperator();
            node->addChild(opNode);

            factor = parseFactor();
            node->addChild(factor);
        } else {
            break;
        }
    }

    return node;
}

// Grammar: <factor> -> IDENTIFIER (DOT IDENTIFIER | LBRACKET <expression> RBRACKET | LPARENTHESIS
// <parameter-list> RPARENTHESIS)* | NUMBER | STRING_LITERAL | CHAR_LITERAL | LPARENTHESIS
// <expression> RPARENTHESIS | LOGICAL_OPERATOR(tidak) <factor> | (ARITHMETIC_OPERATOR(+|-))
// <factor>
shared_ptr<ParseNode> Parser::parseFactor() {
    auto node = make_shared<ParseNode>("factor");

    Token* token = currentToken();

    if (match(ARITHMETIC_OPERATOR) && (token->getValue() == "-" || token->getValue() == "+")) {
        auto unaryNode = make_shared<ParseNode>("ARITHMETIC_OPERATOR", token->getValue());
        node->addChild(unaryNode);
        advance();

        auto factorNode = parseFactor();
        node->addChild(factorNode);
    } else if (match(IDENTIFIER)) {
        auto idNode =
            make_shared<ParseNode>("IDENTIFIER", token->getValue(), token ? token->getLine() : 0);
        node->addChild(idNode);
        advance();

        bool continueLoop = true;
        while (continueLoop) {
            if (match(DOT)) {
                Token* nextToken = peek();
                if (nextToken && nextToken->getType() == DOT) {
                    break;  // range operator not record access
                }

                Token* dotToken = currentToken();
                auto dotNode = make_shared<ParseNode>("DOT", dotToken->getValue());
                node->addChild(dotNode);
                advance();

                token = currentToken();
                if (!match(IDENTIFIER)) {
                    syntaxError("Expected field name after '.'");
                    break;
                }

                auto fieldNode = make_shared<ParseNode>("IDENTIFIER", token->getValue(),
                                                        token ? token->getLine() : 0);
                node->addChild(fieldNode);
                advance();
            } else if (match(LBRACKET)) {
                Token* lbracketToken = currentToken();
                auto lbracketNode = make_shared<ParseNode>("LBRACKET", lbracketToken->getValue());
                node->addChild(lbracketNode);
                advance();

                auto indexExpr = parseExpression();
                node->addChild(indexExpr);

                Token* rbracketToken = currentToken();
                expect(RBRACKET, "Expected ']' after array index");
                auto rbracketNode = make_shared<ParseNode>("RBRACKET", rbracketToken->getValue());
                node->addChild(rbracketNode);
            } else if (match(LPARENTHESIS)) {
                auto paramList = parseParameterList();
                node->addChild(paramList);
                continueLoop = false;
            } else {
                continueLoop = false;
            }
        }
    } else if (match(NUMBER)) {
        string numValue = token->getValue();
        advance();

        if (match(DOT)) {
            Token* nextToken = peek();
            if (nextToken && nextToken->getType() == NUMBER) {
                advance();  // skip DOT
                numValue += ".";
                numValue += currentToken()->getValue();
                advance();  // skip fractional part
            }
        }

        auto numNode = make_shared<ParseNode>("NUMBER", numValue);
        node->addChild(numNode);
    } else if (match(STRING_LITERAL) || match(CHAR_LITERAL)) {
        auto strNode = make_shared<ParseNode>("STRING_LITERAL", token->getValue());
        node->addChild(strNode);
        advance();
    } else if (match(LPARENTHESIS)) {
        Token* lparenToken = currentToken();
        auto lparenNode = make_shared<ParseNode>("LPARENTHESIS", lparenToken->getValue());
        node->addChild(lparenNode);
        advance();

        auto exprNode = parseExpression();
        node->addChild(exprNode);

        Token* rparenToken = currentToken();
        expect(RPARENTHESIS, "Expected ')' after expression");
        auto rparenNode = make_shared<ParseNode>("RPARENTHESIS", rparenToken->getValue());
        node->addChild(rparenNode);
    } else if (match(LOGICAL_OPERATOR) && token->getValue() == "tidak") {
        auto notNode = make_shared<ParseNode>("LOGICAL_OPERATOR", "tidak");
        node->addChild(notNode);
        advance();
        auto factorNode = parseFactor();
        node->addChild(factorNode);
    } else if (matchKeyword("true") || matchKeyword("false")) {
        auto boolNode = make_shared<ParseNode>("KEYWORD", token->getValue());
        node->addChild(boolNode);
        advance();
    } else {
        syntaxError("Expected factor (identifier, number, string, or expression)");
    }

    return node;
}

// Grammar: <relational-operator> -> RELATIONAL_OPERATOR(=|<>|<|<=|>|>=)
shared_ptr<ParseNode> Parser::parseRelationalOperator() {
    Token* token = currentToken();
    if (match(RELATIONAL_OPERATOR)) {
        auto opNode = make_shared<ParseNode>("RELATIONAL_OPERATOR", token->getValue());
        advance();
        return opNode;
    } else {
        syntaxError("Expected relational operator");
        return make_shared<ParseNode>("relational-operator");
    }
}

// Grammar: <additive-operator> -> ARITHMETIC_OPERATOR(+|-) | LOGICAL_OPERATOR(atau)
shared_ptr<ParseNode> Parser::parseAdditiveOperator() {
    Token* token = currentToken();
    string op = token->getValue();

    if (match(ARITHMETIC_OPERATOR) && (op == "+" || op == "-")) {
        auto opNode = make_shared<ParseNode>("ARITHMETIC_OPERATOR", op);
        advance();
        return opNode;
    } else if (match(LOGICAL_OPERATOR) && op == "atau") {
        auto opNode = make_shared<ParseNode>("LOGICAL_OPERATOR", op);
        advance();
        return opNode;
    } else {
        syntaxError("Expected additive operator (+, -, atau)");
        return make_shared<ParseNode>("additive-operator");
    }
}

// Grammar: <multiplication-operator> -> ARITHMETIC_OPERATOR(*|/|bagi|mod) | LOGICAL_OPERATOR(dan)
shared_ptr<ParseNode> Parser::parseMultiplicationOperator() {
    Token* token = currentToken();
    string op = token->getValue();

    if (match(ARITHMETIC_OPERATOR)) {
        auto opNode = make_shared<ParseNode>("ARITHMETIC_OPERATOR", op);
        advance();
        return opNode;
    } else if (match(LOGICAL_OPERATOR) && op == "dan") {
        auto opNode = make_shared<ParseNode>("LOGICAL_OPERATOR", op);
        advance();
        return opNode;
    } else {
        syntaxError("Expected multiplication operator (*, /, bagi, mod, dan)");
        return make_shared<ParseNode>("multiplication-operator");
    }
}
