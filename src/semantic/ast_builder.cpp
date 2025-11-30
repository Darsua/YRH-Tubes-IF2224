#include "include/ast_builder.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

// ==================== CONSTRUCTOR ====================

ASTBuilder::ASTBuilder() {}

// ==================== HELPER FUNCTIONS ====================

bool ASTBuilder::isTerminal(shared_ptr<ParseNode> node) const {
    if (!node) return false;
    string type = node->getType();

    // Terminal nodes are tokens (all uppercase or special symbols)
    return type == "KEYWORD" || type == "IDENTIFIER" || type == "NUMBER" ||
           type == "STRING_LITERAL" || type == "CHAR_LITERAL" || type == "SEMICOLON" ||
           type == "COMMA" || type == "COLON" || type == "DOT" || type == "LPARENTHESIS" ||
           type == "RPARENTHESIS" || type == "LBRACKET" || type == "RBRACKET" ||
           type == "ASSIGN_OPERATOR" || type == "ARITHMETIC_OPERATOR" ||
           type == "RELATIONAL_OPERATOR" || type == "LOGICAL_OPERATOR" || type == "RANGE_OPERATOR";
}

string ASTBuilder::getTokenValue(shared_ptr<ParseNode> node) const {
    if (!node) return "";
    return node->getValue();
}

string ASTBuilder::getNodeType(shared_ptr<ParseNode> node) const {
    if (!node) return "";
    return node->getType();
}

shared_ptr<ParseNode> ASTBuilder::findChild(shared_ptr<ParseNode> node, const string& type) const {
    if (!node) return nullptr;

    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

vector<shared_ptr<ParseNode>> ASTBuilder::findChildren(shared_ptr<ParseNode> node,
                                                       const string& type) const {
    vector<shared_ptr<ParseNode>> result;
    if (!node) return result;

    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

DataType ASTBuilder::stringToDataType(const string& typeStr) {
    string lower = typeStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "integer") return DataType::INTEGER;
    if (lower == "real") return DataType::REAL;
    if (lower == "boolean") return DataType::BOOLEAN;
    if (lower == "char") return DataType::CHAR;
    if (lower == "string") return DataType::STRING;

    return DataType::UNKNOWN;
}

void ASTBuilder::error(const string& message) {
    errors.push_back("AST Builder Error: " + message);
}

void ASTBuilder::setPosition(shared_ptr<ASTNode> astNode, shared_ptr<ParseNode> parseNode) {
    if (!astNode || !parseNode) return;

    // Set position from parse node
    if (parseNode->getLine() > 0) {
        astNode->setLineNumber(parseNode->getLine());
    } else {
        // Try to find position from first child with position info
        for (const auto& child : parseNode->getChildren()) {
            if (child && child->getLine() > 0) {
                astNode->setLineNumber(child->getLine());
                break;
            }
        }
    }
}

int ASTBuilder::extractNumberFromExpression(shared_ptr<ParseNode> expr) const {
    // Navigate through expression tree to find NUMBER terminal
    // <expression> → <simple-expression> → <term> → <factor> → NUMBER
    if (!expr) return 0;

    if (expr->getType() == "NUMBER") {
        return stoi(expr->getValue());
    }

    // Recursively search children
    for (const auto& child : expr->getChildren()) {
        int result = extractNumberFromExpression(child);
        if (result != 0) return result;
    }

    return 0;
}

// ==================== MAIN BUILD FUNCTION ====================

shared_ptr<ProgramNode> ASTBuilder::buildAST(shared_ptr<ParseNode> tree) {
    parseTree = tree;
    errors.clear();

    if (!parseTree) {
        error("Parse tree is null");
        return nullptr;
    }

    return convertProgram(parseTree);
}

// ==================== PROGRAM CONVERSION ====================

shared_ptr<ProgramNode> ASTBuilder::convertProgram(shared_ptr<ParseNode> node) {
    // <program> → program ID ; <declaration-part> <compound-statement> .

    if (!node || node->getType() != "program") {
        error("Expected program node");
        return nullptr;
    }

    // Find program name - look in program-header or direct children
    string programName = "Unknown";

    // First try to find program-header
    auto programHeader = findChild(node, "program-header");
    if (programHeader) {
        for (const auto& child : programHeader->getChildren()) {
            if (child && child->getType() == "IDENTIFIER") {
                programName = child->getValue();
                break;
            }
        }
    } else {
        // Try direct children
        for (const auto& child : node->getChildren()) {
            if (child && child->getType() == "IDENTIFIER") {
                programName = child->getValue();
                break;
            }
        }
    }

    auto programNode = make_shared<ProgramNode>(programName);
    setPosition(programNode, node);  // Set line number from program node

    // Find declaration part
    auto declPart = findChild(node, "declaration-part");
    if (declPart) {
        auto declarations = convertDeclarationPart(declPart);
        for (auto& decl : declarations) {
            programNode->addDeclaration(decl);
        }
    }

    // Find compound statement
    auto compStmt = findChild(node, "compound-statement");
    if (compStmt) {
        programNode->setCompoundStatement(convertCompoundStatement(compStmt));
    }

    return programNode;
}

// ==================== DECLARATION CONVERSIONS ====================

vector<shared_ptr<ASTNode>> ASTBuilder::convertDeclarationPart(shared_ptr<ParseNode> node) {
    vector<shared_ptr<ASTNode>> declarations;

    if (!node) return declarations;

    // Find all const, type, var, procedure, function declarations
    for (const auto& child : node->getChildren()) {
        if (!child) continue;

        string type = child->getType();

        if (type == "const-declaration") {
            auto constDecls = convertConstDeclaration(child);
            declarations.insert(declarations.end(), constDecls.begin(), constDecls.end());
        } else if (type == "type-declaration") {
            auto typeDecls = convertTypeDeclaration(child);
            declarations.insert(declarations.end(), typeDecls.begin(), typeDecls.end());
        } else if (type == "var-declaration") {
            auto varDecls = convertVarDeclaration(child);
            declarations.insert(declarations.end(), varDecls.begin(), varDecls.end());
        } else if (type == "subprogram-declaration") {
            // Check if it's procedure or function
            auto procDecl = findChild(child, "procedure-declaration");
            auto funcDecl = findChild(child, "function-declaration");

            if (procDecl) {
                declarations.push_back(convertProcedureDeclaration(procDecl));
            } else if (funcDecl) {
                declarations.push_back(convertFunctionDeclaration(funcDecl));
            }
        }
    }

    return declarations;
}

vector<shared_ptr<VarDeclNode>> ASTBuilder::convertVarDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<VarDeclNode>> varDecls;

    if (!node) return varDecls;

    // var-declaration can have multiple groups:
    // <identifier-list> : <type> ; <identifier-list> : <type> ; ...
    const auto& children = node->getChildren();

    vector<pair<string, shared_ptr<ParseNode>>>
        currentIdentifiers;  // name and parse node for position
    DataType currentType = DataType::UNKNOWN;
    string customTypeName = "";

    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "identifier-list") {
            // Extract identifiers from this list with their positions
            currentIdentifiers.clear();
            for (const auto& idChild : child->getChildren()) {
                if (idChild && idChild->getType() == "IDENTIFIER") {
                    currentIdentifiers.push_back({idChild->getValue(), idChild});
                }
            }
        } else if (child->getType() == "type") {
            // Check if type is a custom type (IDENTIFIER) or primitive (KEYWORD)
            customTypeName = "";
            auto customTypeNode = findChild(child, "custom-type");
            if (customTypeNode) {
                // custom-type node directly contains the type name as value
                customTypeName = customTypeNode->getValue();
            }

            // Get type for current identifier list
            currentType = convertType(child);

            // Create VarDeclNode for each identifier in current group
            for (const auto& identPair : currentIdentifiers) {
                auto varDecl = make_shared<VarDeclNode>(identPair.first, currentType);
                if (!customTypeName.empty()) {
                    varDecl->setCustomTypeName(customTypeName);
                }
                setPosition(varDecl, identPair.second);  // Set position from identifier token
                varDecls.push_back(varDecl);
            }

            // Reset for next group
            currentIdentifiers.clear();
            currentType = DataType::UNKNOWN;
            customTypeName = "";
        }
    }

    return varDecls;
}

vector<shared_ptr<ConstDeclNode>> ASTBuilder::convertConstDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<ConstDeclNode>> constDecls;

    if (!node) return constDecls;

    // const ID = <constant-value> ; [ID = <constant-value> ;]*
    const auto& children = node->getChildren();

    for (size_t i = 0; i < children.size(); i++) {
        if (children[i] && children[i]->getType() == "IDENTIFIER") {
            string constName = children[i]->getValue();

            // Look for value after '=' (should be <constant-value> node)
            shared_ptr<ASTNode> value;

            if (i + 2 < children.size()) {
                auto valNode = children[i + 2];

                // Check if it's <constant-value> wrapper
                if (valNode->getType() == "constant-value") {
                    // Extract actual value from constant-value node
                    string valStr = valNode->getValue();

                    // Try to determine type from value string
                    if (!valStr.empty()) {
                        // Check if it's a string (has quotes)
                        if ((valStr.front() == '\'' && valStr.back() == '\'') ||
                            (valStr.front() == '"' && valStr.back() == '"')) {
                            // String literal - remove quotes
                            value = make_shared<StringNode>(valStr.substr(1, valStr.length() - 2));
                            setPosition(value, valNode);
                        }
                        // Check if it's a number
                        else if (isdigit(valStr[0]) || (valStr[0] == '-' && valStr.length() > 1)) {
                            if (valStr.find('.') != string::npos) {
                                value = make_shared<NumberNode>(stod(valStr));
                            } else {
                                value = make_shared<NumberNode>(stoi(valStr));
                            }
                            setPosition(value, valNode);
                        }
                        // Check if it's boolean
                        else if (valStr == "true" || valStr == "false") {
                            value = make_shared<BoolNode>(valStr == "true");
                            setPosition(value, valNode);
                        }
                        // Single character
                        else if (valStr.length() == 1) {
                            value = make_shared<CharNode>(valStr[0]);
                            setPosition(value, valNode);
                        }
                    }
                }
                // Fallback: direct value nodes (if parser structure changes)
                else if (valNode->getType() == "NUMBER") {
                    string numStr = valNode->getValue();
                    if (numStr.find('.') != string::npos) {
                        value = make_shared<NumberNode>(stod(numStr));
                    } else {
                        value = make_shared<NumberNode>(stoi(numStr));
                    }
                    setPosition(value, valNode);
                } else if (valNode->getType() == "STRING_LITERAL") {
                    value = make_shared<StringNode>(valNode->getValue());
                    setPosition(value, valNode);
                } else if (valNode->getType() == "CHAR_LITERAL") {
                    string charStr = valNode->getValue();
                    if (!charStr.empty()) {
                        value = make_shared<CharNode>(charStr[0]);
                        setPosition(value, valNode);
                    }
                }
            }

            if (value) {
                auto constDecl = make_shared<ConstDeclNode>(constName, value);
                setPosition(constDecl, children[i]);  // Set line from IDENTIFIER node
                constDecls.push_back(constDecl);
            }
        }
    }

    return constDecls;
}

vector<shared_ptr<TypeDeclNode>> ASTBuilder::convertTypeDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<TypeDeclNode>> typeDecls;

    if (!node) return typeDecls;

    // type-declaration contains multiple <type-definition> children
    const auto& children = node->getChildren();

    for (const auto& child : children) {
        if (!child || child->getType() != "type-definition") continue;

        // <type-definition>: IDENTIFIER = <type> ;
        const auto& defChildren = child->getChildren();

        string typeName;
        shared_ptr<ParseNode> typeNode;

        // Extract IDENTIFIER and <type> from type-definition
        for (const auto& defChild : defChildren) {
            if (!defChild) continue;

            if (defChild->getType() == "IDENTIFIER") {
                typeName = defChild->getValue();
            } else if (defChild->getType() == "type") {
                typeNode = defChild;
            }
        }

        if (typeName.empty() || !typeNode) continue;

        // Check what kind of type it is
        auto arrayType = findChild(typeNode, "array-type");
        auto recordType = findChild(typeNode, "record-type");

        if (arrayType) {
            // Array type
            auto arrayNode = convertArrayType(arrayType);
            auto typeDecl = make_shared<TypeDeclNode>(typeName, DataType::ARRAY, arrayNode);
            setPosition(typeDecl, child);  // Set line from type-definition node
            typeDecls.push_back(typeDecl);
        } else if (recordType) {
            // Record type
            auto recordNode = convertRecordType(recordType);
            auto typeDecl = make_shared<TypeDeclNode>(typeName, DataType::RECORD, recordNode);
            setPosition(typeDecl, child);
            typeDecls.push_back(typeDecl);
        } else {
            // Simple type alias (e.g., NilaiRange = integer)
            DataType baseType = convertType(typeNode);
            auto typeDecl = make_shared<TypeDeclNode>(typeName, baseType, nullptr);
            setPosition(typeDecl, child);
            typeDecls.push_back(typeDecl);
        }
    }

    return typeDecls;
}

shared_ptr<ProcedureDeclNode> ASTBuilder::convertProcedureDeclaration(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // procedure ID [(<formal-parameter-list>)] ; <declaration-part> <compound-statement> ;

    // Find procedure name
    string procName = "Unknown";
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "IDENTIFIER") {
            procName = child->getValue();
            break;
        }
    }

    auto procDecl = make_shared<ProcedureDeclNode>(procName);
    setPosition(procDecl, node);  // Set line from procedure-declaration node

    // Find formal parameters
    auto paramList = findChild(node, "formal-parameter-list");
    if (paramList) {
        auto params = convertFormalParameterList(paramList);
        for (auto& param : params) {
            procDecl->addParameter(param);
        }
    }

    // Find local declarations
    auto declPart = findChild(node, "declaration-part");
    if (declPart) {
        auto declarations = convertDeclarationPart(declPart);
        for (auto& decl : declarations) {
            procDecl->addDeclaration(decl);
        }
    }

    // Find compound statement (body)
    auto compStmt = findChild(node, "compound-statement");
    if (compStmt) {
        procDecl->setCompoundStatement(convertCompoundStatement(compStmt));
    }

    return procDecl;
}

shared_ptr<FunctionDeclNode> ASTBuilder::convertFunctionDeclaration(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // function ID [(<formal-parameter-list>)] : <type> ; <declaration-part> <compound-statement> ;

    // Find function name
    string funcName = "Unknown";
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "IDENTIFIER") {
            funcName = child->getValue();
            break;
        }
    }

    // Find return type
    auto typeNode = findChild(node, "type");
    DataType returnType = typeNode ? convertType(typeNode) : DataType::UNKNOWN;

    auto funcDecl = make_shared<FunctionDeclNode>(funcName, returnType);
    setPosition(funcDecl, node);  // Set line from function-declaration node

    // Find formal parameters
    auto paramList = findChild(node, "formal-parameter-list");
    if (paramList) {
        auto params = convertFormalParameterList(paramList);
        for (auto& param : params) {
            funcDecl->addParameter(param);
        }
    }

    // Find local declarations
    auto declPart = findChild(node, "declaration-part");
    if (declPart) {
        auto declarations = convertDeclarationPart(declPart);
        for (auto& decl : declarations) {
            funcDecl->addDeclaration(decl);
        }
    }

    // Find compound statement (body)
    auto compStmt = findChild(node, "compound-statement");
    if (compStmt) {
        funcDecl->setCompoundStatement(convertCompoundStatement(compStmt));
    }

    return funcDecl;
}

// ==================== TYPE CONVERSIONS ====================

DataType ASTBuilder::convertType(shared_ptr<ParseNode> node) {
    if (!node) return DataType::UNKNOWN;

    // Check for simple type (KEYWORD)
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "KEYWORD") {
            return stringToDataType(child->getValue());
        }

        // Check for custom type (IDENTIFIER) - indicates user-defined type
        if (child && child->getType() == "custom-type") {
            // Custom types like DaftarNilai, Matrix2D, etc.
            // Return UNKNOWN to signal that customTypeName should be checked
            return DataType::UNKNOWN;
        }

        // Check for array type
        if (child && child->getType() == "array-type") {
            return DataType::ARRAY;
        }

        // Check for record type
        if (child && child->getType() == "record-type") {
            return DataType::RECORD;
        }
    }

    return DataType::UNKNOWN;
}

shared_ptr<ArrayTypeNode> ASTBuilder::convertArrayType(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // array [ <range> ] of <type>
    auto rangeNode = findChild(node, "range");
    auto typeNode = findChild(node, "type");

    if (!rangeNode || !typeNode) return nullptr;

    // Get bounds from range
    // <range>: <expression> .. <expression>
    int lowBound = 0, highBound = 0;
    const auto& rangeChildren = rangeNode->getChildren();

    // Find the two expressions (separated by RANGE_OPERATOR)
    bool foundFirst = false;
    for (const auto& child : rangeChildren) {
        if (!child) continue;

        if (child->getType() == "expression") {
            // Extract number from nested expression structure
            int value = extractNumberFromExpression(child);
            if (!foundFirst) {
                lowBound = value;
                foundFirst = true;
            } else {
                highBound = value;
                break;
            }
        }
    }

    // Check if element type is nested array
    auto nestedArrayNode = findChild(typeNode, "array-type");
    if (nestedArrayNode) {
        // Recursive case: multi-dimensional array
        auto nestedArray = convertArrayType(nestedArrayNode);
        return make_shared<ArrayTypeNode>(lowBound, highBound, nestedArray);
    } else {
        // Base case: simple element type
        DataType elemType = convertType(typeNode);
        return make_shared<ArrayTypeNode>(lowBound, highBound, elemType);
    }
}

shared_ptr<RecordTypeNode> ASTBuilder::convertRecordType(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // record <field-list> end
    auto recordNode = make_shared<RecordTypeNode>();

    // Find all var-declaration nodes (fields)
    auto varDecls = findChildren(node, "var-declaration");
    for (const auto& varDecl : varDecls) {
        auto fields = convertVarDeclaration(varDecl);
        for (auto& field : fields) {
            recordNode->addField(field);
        }
    }

    return recordNode;
}

vector<shared_ptr<VarDeclNode>> ASTBuilder::convertFormalParameterList(shared_ptr<ParseNode> node) {
    vector<shared_ptr<VarDeclNode>> params;

    if (!node) return params;

    // Parameters: ( <identifier-list> : <type> (; <identifier-list> : <type>)* )
    const auto& children = node->getChildren();

    bool isVarParam = false;
    vector<string> identifiers;
    DataType paramType = DataType::UNKNOWN;

    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "KEYWORD" && child->getValue() == "variabel") {
            isVarParam = true;
        } else if (child->getType() == "identifier-list") {
            // Extract identifiers from identifier-list node
            for (const auto& idChild : child->getChildren()) {
                if (idChild && idChild->getType() == "IDENTIFIER") {
                    identifiers.push_back(idChild->getValue());
                }
            }
        } else if (child->getType() == "type") {
            paramType = convertType(child);

            // Create parameters for all identifiers in this group
            for (const auto& ident : identifiers) {
                auto param = make_shared<VarDeclNode>(ident, paramType);
                param->setIsParameter(true);
                param->setIsVarParameter(isVarParam);
                params.push_back(param);
            }

            // Reset for next parameter group
            identifiers.clear();
            isVarParam = false;
        }
    }

    return params;
}

// ==================== STATEMENT CONVERSIONS ====================

shared_ptr<CompoundStatementNode> ASTBuilder::convertCompoundStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // begin <statement-list> end
    auto compStmt = make_shared<CompoundStatementNode>();

    auto stmtList = findChild(node, "statement-list");
    if (stmtList) {
        auto statements = convertStatementList(stmtList);
        for (auto& stmt : statements) {
            if (stmt) {
                compStmt->addStatement(stmt);
            }
        }
    }

    return compStmt;
}

vector<shared_ptr<ASTNode>> ASTBuilder::convertStatementList(shared_ptr<ParseNode> node) {
    vector<shared_ptr<ASTNode>> statements;

    if (!node) return statements;

    // Recursively collect all statements
    for (const auto& child : node->getChildren()) {
        if (!child) continue;

        if (child->getType() == "assignment-statement" || child->getType() == "if-statement" ||
            child->getType() == "while-statement" || child->getType() == "for-statement" ||
            child->getType() == "procedure-call" || child->getType() == "compound-statement") {
            auto stmt = convertStatement(child);
            if (stmt) {
                statements.push_back(stmt);
            }
        } else if (child->getType() == "statement-list") {
            // Recursive case
            auto subStmts = convertStatementList(child);
            statements.insert(statements.end(), subStmts.begin(), subStmts.end());
        }
    }

    return statements;
}

shared_ptr<ASTNode> ASTBuilder::convertStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    string type = node->getType();

    // If it's a generic "statement" wrapper, look inside for the actual statement
    if (type == "statement") {
        // Find the first child that is an actual statement type
        for (const auto& child : node->getChildren()) {
            if (child) {
                auto stmt = convertStatement(child);
                if (stmt) return stmt;
            }
        }
        return nullptr;
    }

    if (type == "assignment-statement") {
        return convertAssignmentStatement(node);
    } else if (type == "if-statement") {
        return convertIfStatement(node);
    } else if (type == "while-statement") {
        return convertWhileStatement(node);
    } else if (type == "for-statement") {
        return convertForStatement(node);
    } else if (type == "procedure-call") {
        return convertProcedureCall(node);
    } else if (type == "compound-statement") {
        return convertCompoundStatement(node);
    }

    return nullptr;
}

shared_ptr<AssignNode> ASTBuilder::convertAssignmentStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // <variable> := <expression>

    // Find target (variable reference)
    shared_ptr<ASTNode> target;
    auto identifier = findChild(node, "IDENTIFIER");
    if (identifier) {
        target = make_shared<VarNode>(identifier->getValue());
        setPosition(target, identifier);

        // Check for array access or record access
        // This would require more parsing of the parse tree structure
        // For now, simple variable reference
    }

    // Find expression
    auto exprNode = findChild(node, "expression");
    shared_ptr<ASTNode> value = exprNode ? convertExpression(exprNode) : nullptr;

    if (!target || !value) return nullptr;

    auto assignNode = make_shared<AssignNode>(target, value);
    setPosition(assignNode, node);  // Set line from assignment-statement node
    return assignNode;
}

shared_ptr<IfNode> ASTBuilder::convertIfStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // if <expression> then <statement> [else <statement>]

    auto exprNode = findChild(node, "expression");
    shared_ptr<ASTNode> condition = exprNode ? convertExpression(exprNode) : nullptr;

    if (!condition) return nullptr;

    // Find then and else statements
    const auto& children = node->getChildren();
    shared_ptr<ASTNode> thenStmt = nullptr;
    shared_ptr<ASTNode> elseStmt = nullptr;

    bool foundThen = false;
    bool foundElse = false;

    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "KEYWORD" && child->getValue() == "maka") {
            foundThen = true;
            continue;
        }
        if (child->getType() == "KEYWORD" && child->getValue() == "selain_itu") {
            foundElse = true;
            continue;
        }

        if (foundThen && !foundElse && !thenStmt) {
            // The next child should be <statement> or a statement type
            if (child->getType() == "statement" || child->getType() == "assignment-statement" ||
                child->getType() == "compound-statement" || child->getType() == "if-statement" ||
                child->getType() == "while-statement" || child->getType() == "for-statement" ||
                child->getType() == "procedure-call") {
                thenStmt = convertStatement(child);
            }
        } else if (foundElse && !elseStmt) {
            // The next child should be <statement> or a statement type
            if (child->getType() == "statement" || child->getType() == "assignment-statement" ||
                child->getType() == "compound-statement" || child->getType() == "if-statement" ||
                child->getType() == "while-statement" || child->getType() == "for-statement" ||
                child->getType() == "procedure-call") {
                elseStmt = convertStatement(child);
            }
        }
    }

    return make_shared<IfNode>(condition, thenStmt, elseStmt);
}

shared_ptr<WhileNode> ASTBuilder::convertWhileStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // while <expression> do <statement>

    auto exprNode = findChild(node, "expression");
    shared_ptr<ASTNode> condition = exprNode ? convertExpression(exprNode) : nullptr;

    if (!condition) return nullptr;

    // Find statement after 'lakukan'
    const auto& children = node->getChildren();
    shared_ptr<ASTNode> body = nullptr;

    bool foundDo = false;
    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "KEYWORD" && child->getValue() == "lakukan") {
            foundDo = true;
            continue;
        }

        if (foundDo) {
            // The next child should be <statement> or a statement type
            if (child->getType() == "statement" || child->getType() == "assignment-statement" ||
                child->getType() == "compound-statement" || child->getType() == "if-statement" ||
                child->getType() == "while-statement" || child->getType() == "for-statement" ||
                child->getType() == "procedure-call") {
                body = convertStatement(child);
                break;
            }
        }
    }

    return make_shared<WhileNode>(condition, body);
}

shared_ptr<ForNode> ASTBuilder::convertForStatement(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // for ID := <expression> to|downto <expression> do <statement>

    string loopVar = "";
    auto identifier = findChild(node, "IDENTIFIER");
    if (identifier) {
        loopVar = identifier->getValue();
    }

    // Find start and end expressions
    auto expressions = findChildren(node, "expression");
    shared_ptr<ASTNode> startExpr =
        expressions.size() > 0 ? convertExpression(expressions[0]) : nullptr;
    shared_ptr<ASTNode> endExpr =
        expressions.size() > 1 ? convertExpression(expressions[1]) : nullptr;

    // Check for 'to' or 'downto'
    bool isDownto = false;
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "KEYWORD") {
            if (child->getValue() == "turun_ke") {
                isDownto = true;
                break;
            }
        }
    }

    // Find body (the statement after 'lakukan')
    const auto& children = node->getChildren();
    shared_ptr<ASTNode> body = nullptr;

    // Look for <statement> node after KEYWORD(lakukan)
    bool foundDo = false;
    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "KEYWORD" && child->getValue() == "lakukan") {
            foundDo = true;
            continue;
        }

        if (foundDo) {
            // The next child should be <statement> or a statement type
            if (child->getType() == "statement" || child->getType() == "assignment-statement" ||
                child->getType() == "compound-statement" || child->getType() == "if-statement" ||
                child->getType() == "while-statement" || child->getType() == "for-statement" ||
                child->getType() == "procedure-call") {
                body = convertStatement(child);
                break;
            }
        }
    }

    return make_shared<ForNode>(loopVar, startExpr, endExpr, isDownto, body);
}

shared_ptr<ProcCallNode> ASTBuilder::convertProcedureCall(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // ID [(<parameter-list>)]

    string procName = "";
    auto identifier = findChild(node, "IDENTIFIER");
    if (identifier) {
        procName = identifier->getValue();
    }

    auto procCall = make_shared<ProcCallNode>(procName);
    // Set position from identifier if available, otherwise from the whole node
    if (identifier)
        setPosition(procCall, identifier);
    else
        setPosition(procCall, node);

    // Find parameter list
    auto paramList = findChild(node, "parameter-list");
    if (paramList) {
        auto expressions = findChildren(paramList, "expression");
        for (auto& exprNode : expressions) {
            auto arg = convertExpression(exprNode);
            if (arg) {
                procCall->addArgument(arg);
            }
        }
    }

    return procCall;
}

// ==================== EXPRESSION CONVERSIONS ====================

shared_ptr<ASTNode> ASTBuilder::convertExpression(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // <expression> → <simple-expression> [relop <simple-expression>]

    auto simpleExprs = findChildren(node, "simple-expression");

    if (simpleExprs.size() == 1) {
        return convertSimpleExpression(simpleExprs[0]);
    } else if (simpleExprs.size() == 2) {
        // Binary operation with relational operator
        auto left = convertSimpleExpression(simpleExprs[0]);
        auto right = convertSimpleExpression(simpleExprs[1]);

        // Find relational operator (RELATIONAL_OPERATOR token)
        string op = "";
        for (const auto& child : node->getChildren()) {
            if (child && child->getType() == "RELATIONAL_OPERATOR") {
                op = child->getValue();
                break;
            }
        }

        {
            auto bin = make_shared<BinOpNode>(op, left, right);
            setPosition(bin, node);
            return bin;
        }
        // Ensure binop has a position (from this expression node)
        // Note: we create it above and want to set its position before returning
        // However previous line returned directly; adjust to set position.
    }

    return nullptr;
}

shared_ptr<ASTNode> ASTBuilder::convertSimpleExpression(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // <simple-expression> → [sign] <term> {addop <term>}

    const auto& children = node->getChildren();

    // Collect terms and operators in order
    vector<shared_ptr<ParseNode>> terms;
    vector<string> operators;

    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "term") {
            terms.push_back(child);
        } else if (child->getType() == "ARITHMETIC_OPERATOR") {
            // Additive operators: +, -, or
            operators.push_back(child->getValue());
        } else if (child->getType() == "LOGICAL_OPERATOR") {
            // 'or' is also an additive operator
            string op = child->getValue();
            if (op == "or" || op == "atau") {
                operators.push_back(op);
            }
        }
    }

    if (terms.empty()) return nullptr;

    // Start with first term
    shared_ptr<ASTNode> result = convertTerm(terms[0]);

    // Combine terms with operators (left-to-right)
    for (size_t i = 1; i < terms.size() && i - 1 < operators.size(); i++) {
        auto right = convertTerm(terms[i]);
        auto bin = make_shared<BinOpNode>(operators[i - 1], result, right);
        setPosition(bin, node);
        result = bin;
    }

    return result;
}

shared_ptr<ASTNode> ASTBuilder::convertTerm(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // <term> → <factor> {mulop <factor>}

    const auto& children = node->getChildren();

    // Collect factors and operators in order
    vector<shared_ptr<ParseNode>> factors;
    vector<string> operators;

    for (const auto& child : children) {
        if (!child) continue;

        if (child->getType() == "factor") {
            factors.push_back(child);
        } else if (child->getType() == "ARITHMETIC_OPERATOR") {
            // Multiplicative operators: *, bagi, mod, div
            string op = child->getValue();
            if (op == "*" || op == "bagi" || op == "mod" || op == "div" || op == "/") {
                operators.push_back(op);
            }
        } else if (child->getType() == "LOGICAL_OPERATOR") {
            // 'and' is also a multiplicative operator
            string op = child->getValue();
            if (op == "and" || op == "dan") {
                operators.push_back(op);
            }
        }
    }

    if (factors.empty()) return nullptr;

    // Start with first factor
    shared_ptr<ASTNode> result = convertFactor(factors[0]);

    // Combine factors with operators (left-to-right)
    for (size_t i = 1; i < factors.size() && i - 1 < operators.size(); i++) {
        auto right = convertFactor(factors[i]);
        auto bin = make_shared<BinOpNode>(operators[i - 1], result, right);
        setPosition(bin, node);
        result = bin;
    }

    return result;
}

shared_ptr<ASTNode> ASTBuilder::convertFactor(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;

    // <factor> → ID | NUMBER | STRING | CHAR | ( <expression> ) | not <factor>

    for (const auto& child : node->getChildren()) {
        if (!child) continue;

        if (child->getType() == "IDENTIFIER") {
            // Could be variable, constant, or function call
            // For now, treat as variable reference
            auto varNode = make_shared<VarNode>(child->getValue());
            setPosition(varNode, child);
            return varNode;
        } else if (child->getType() == "NUMBER") {
            string numStr = child->getValue();
            shared_ptr<ASTNode> numNode;
            if (numStr.find('.') != string::npos) {
                numNode = make_shared<NumberNode>(stod(numStr));
            } else {
                numNode = make_shared<NumberNode>(stoi(numStr));
            }
            setPosition(numNode, child);
            return numNode;
        } else if (child->getType() == "STRING_LITERAL") {
            auto strNode = make_shared<StringNode>(child->getValue());
            setPosition(strNode, child);
            return strNode;
        } else if (child->getType() == "CHAR_LITERAL") {
            string charStr = child->getValue();
            if (!charStr.empty()) {
                auto cnode = make_shared<CharNode>(charStr[0]);
                setPosition(cnode, child);
                return cnode;
            }
        } else if (child->getType() == "expression") {
            // Parenthesized expression
            return convertExpression(child);
        } else if (child->getType() == "KEYWORD" && child->getValue() == "not") {
            // Unary not operator
            auto operand = convertFactor(node);
            auto un = make_shared<UnaryOpNode>("not", operand);
            setPosition(un, child);
            return un;
        } else if (child->getType() == "KEYWORD") {
            // Handle boolean literals true/false
            string val = child->getValue();
            if (val == "true" || val == "false") {
                auto b = make_shared<BoolNode>(val == "true");
                setPosition(b, child);
                return b;
            }
        } else if (child->getType() == "factor") {
            // Recursive case
            return convertFactor(child);
        }
    }

    return nullptr;
}

shared_ptr<ASTNode> ASTBuilder::convertVariableAccess(shared_ptr<ParseNode> node) {
    // TODO: Handle array access and record access
    // For now, return simple variable reference

    if (!node) return nullptr;

    auto identifier = findChild(node, "IDENTIFIER");
    if (identifier) {
        auto varNode = make_shared<VarNode>(identifier->getValue());
        setPosition(varNode, identifier);
        return varNode;
    }

    return nullptr;
}
