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
    return type == "KEYWORD" || type == "IDENTIFIER" || 
           type == "NUMBER" || type == "STRING_LITERAL" || 
           type == "CHAR_LITERAL" || type == "SEMICOLON" ||
           type == "COMMA" || type == "COLON" || type == "DOT" ||
           type == "LPARENTHESIS" || type == "RPARENTHESIS" ||
           type == "LBRACKET" || type == "RBRACKET" ||
           type == "ASSIGN_OPERATOR" || type == "ARITHMETIC_OPERATOR" ||
           type == "RELATIONAL_OPERATOR" || type == "LOGICAL_OPERATOR" ||
           type == "RANGE_OPERATOR";
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

vector<shared_ptr<ParseNode>> ASTBuilder::findChildren(shared_ptr<ParseNode> node, const string& type) const {
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
        }
        else if (type == "type-declaration") {
            auto typeDecls = convertTypeDeclaration(child);
            declarations.insert(declarations.end(), typeDecls.begin(), typeDecls.end());
        }
        else if (type == "var-declaration") {
            auto varDecls = convertVarDeclaration(child);
            declarations.insert(declarations.end(), varDecls.begin(), varDecls.end());
        }
        else if (type == "subprogram-declaration") {
            // Check if it's procedure or function
            auto procDecl = findChild(child, "procedure-declaration");
            auto funcDecl = findChild(child, "function-declaration");
            
            if (procDecl) {
                declarations.push_back(convertProcedureDeclaration(procDecl));
            }
            else if (funcDecl) {
                declarations.push_back(convertFunctionDeclaration(funcDecl));
            }
        }
    }
    
    return declarations;
}

vector<shared_ptr<VarDeclNode>> ASTBuilder::convertVarDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<VarDeclNode>> varDecls;
    
    if (!node) return varDecls;
    
    // var <identifier-list> : <type> ;
    auto identList = findChild(node, "identifier-list");
    auto typeNode = findChild(node, "type");
    
    if (!identList || !typeNode) return varDecls;
    
    // Get type
    DataType varType = convertType(typeNode);
    
    // Get all identifiers
    vector<string> identifiers;
    for (const auto& child : identList->getChildren()) {
        if (child && child->getType() == "IDENTIFIER") {
            identifiers.push_back(child->getValue());
        }
    }
    
    // Create VarDeclNode for each identifier
    for (const auto& ident : identifiers) {
        auto varDecl = make_shared<VarDeclNode>(ident, varType);
        varDecls.push_back(varDecl);
    }
    
    return varDecls;
}

vector<shared_ptr<ConstDeclNode>> ASTBuilder::convertConstDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<ConstDeclNode>> constDecls;
    
    if (!node) return constDecls;
    
    // const ID = <constant> ; [ID = <constant> ;]*
    const auto& children = node->getChildren();
    
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i] && children[i]->getType() == "IDENTIFIER") {
            string constName = children[i]->getValue();
            
            // Look for value (NUMBER, STRING_LITERAL, CHAR_LITERAL, or identifier)
            shared_ptr<ASTNode> value;
            
            if (i + 2 < children.size()) {
                auto valNode = children[i + 2];
                
                if (valNode->getType() == "NUMBER") {
                    // Check if it's integer or real
                    string numStr = valNode->getValue();
                    if (numStr.find('.') != string::npos) {
                        value = make_shared<NumberNode>(stod(numStr));
                    } else {
                        value = make_shared<NumberNode>(stoi(numStr));
                    }
                }
                else if (valNode->getType() == "STRING_LITERAL") {
                    value = make_shared<StringNode>(valNode->getValue());
                }
                else if (valNode->getType() == "CHAR_LITERAL") {
                    string charStr = valNode->getValue();
                    if (!charStr.empty()) {
                        value = make_shared<CharNode>(charStr[0]);
                    }
                }
                else if (valNode->getType() == "IDENTIFIER") {
                    // Boolean constants or reference to another constant
                    string val = valNode->getValue();
                    if (val == "true" || val == "false") {
                        value = make_shared<BoolNode>(val == "true");
                    }
                }
            }
            
            if (value) {
                auto constDecl = make_shared<ConstDeclNode>(constName, value);
                constDecls.push_back(constDecl);
            }
        }
    }
    
    return constDecls;
}

vector<shared_ptr<TypeDeclNode>> ASTBuilder::convertTypeDeclaration(shared_ptr<ParseNode> node) {
    vector<shared_ptr<TypeDeclNode>> typeDecls;
    
    if (!node) return typeDecls;
    
    // type ID = <type-definition> ;
    const auto& children = node->getChildren();
    
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i] && children[i]->getType() == "IDENTIFIER") {
            string typeName = children[i]->getValue();
            
            // Look for type definition
            if (i + 2 < children.size()) {
                auto typeDef = children[i + 2];
                
                if (typeDef->getType() == "type-definition") {
                    // Check if it's array or record
                    auto arrayType = findChild(typeDef, "array-type");
                    auto recordType = findChild(typeDef, "record-type");
                    
                    if (arrayType) {
                        auto arrayNode = convertArrayType(arrayType);
                        auto typeDecl = make_shared<TypeDeclNode>(typeName, DataType::ARRAY, arrayNode);
                        typeDecls.push_back(typeDecl);
                    }
                    else if (recordType) {
                        auto recordNode = convertRecordType(recordType);
                        auto typeDecl = make_shared<TypeDeclNode>(typeName, DataType::RECORD, recordNode);
                        typeDecls.push_back(typeDecl);
                    }
                }
            }
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
    
    // Get bounds from range (NUMBER .. NUMBER)
    int lowBound = 0, highBound = 0;
    const auto& rangeChildren = rangeNode->getChildren();
    
    for (size_t i = 0; i < rangeChildren.size(); i++) {
        if (rangeChildren[i] && rangeChildren[i]->getType() == "NUMBER") {
            if (lowBound == 0) {
                lowBound = stoi(rangeChildren[i]->getValue());
            } else {
                highBound = stoi(rangeChildren[i]->getValue());
                break;
            }
        }
    }
    
    // Get element type
    DataType elemType = convertType(typeNode);
    
    return make_shared<ArrayTypeNode>(lowBound, highBound, elemType);
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
        
        if (child->getType() == "KEYWORD" && child->getValue() == "var") {
            isVarParam = true;
        }
        else if (child->getType() == "identifier-list") {
            // Extract identifiers from identifier-list node
            for (const auto& idChild : child->getChildren()) {
                if (idChild && idChild->getType() == "IDENTIFIER") {
                    identifiers.push_back(idChild->getValue());
                }
            }
        }
        else if (child->getType() == "type") {
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
        
        if (child->getType() == "assignment-statement" ||
            child->getType() == "if-statement" ||
            child->getType() == "while-statement" ||
            child->getType() == "for-statement" ||
            child->getType() == "procedure-call" ||
            child->getType() == "compound-statement") {
            
            auto stmt = convertStatement(child);
            if (stmt) {
                statements.push_back(stmt);
            }
        }
        else if (child->getType() == "statement-list") {
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
    
    if (type == "assignment-statement") {
        return convertAssignmentStatement(node);
    }
    else if (type == "if-statement") {
        return convertIfStatement(node);
    }
    else if (type == "while-statement") {
        return convertWhileStatement(node);
    }
    else if (type == "for-statement") {
        return convertForStatement(node);
    }
    else if (type == "procedure-call") {
        return convertProcedureCall(node);
    }
    else if (type == "compound-statement") {
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
        
        // Check for array access or record access
        // This would require more parsing of the parse tree structure
        // For now, simple variable reference
    }
    
    // Find expression
    auto exprNode = findChild(node, "expression");
    shared_ptr<ASTNode> value = exprNode ? convertExpression(exprNode) : nullptr;
    
    if (!target || !value) return nullptr;
    
    return make_shared<AssignNode>(target, value);
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
        
        if (child->getType() == "KEYWORD" && child->getValue() == "then") {
            foundThen = true;
            continue;
        }
        if (child->getType() == "KEYWORD" && child->getValue() == "else") {
            foundElse = true;
            continue;
        }
        
        if (foundThen && !foundElse && !thenStmt) {
            if (child->getType() != "KEYWORD" && child->getType() != "expression") {
                thenStmt = convertStatement(child);
            }
        }
        else if (foundElse && !elseStmt) {
            if (child->getType() != "KEYWORD") {
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
    
    // Find statement after 'do'
    const auto& children = node->getChildren();
    shared_ptr<ASTNode> body = nullptr;
    
    bool foundDo = false;
    for (const auto& child : children) {
        if (!child) continue;
        
        if (child->getType() == "KEYWORD" && child->getValue() == "do") {
            foundDo = true;
            continue;
        }
        
        if (foundDo && !body) {
            if (child->getType() != "KEYWORD" && child->getType() != "expression") {
                body = convertStatement(child);
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
    shared_ptr<ASTNode> startExpr = expressions.size() > 0 ? convertExpression(expressions[0]) : nullptr;
    shared_ptr<ASTNode> endExpr = expressions.size() > 1 ? convertExpression(expressions[1]) : nullptr;
    
    // Check for 'to' or 'downto'
    bool isDownto = false;
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "KEYWORD") {
            if (child->getValue() == "downto") {
                isDownto = true;
                break;
            }
        }
    }
    
    // Find body
    const auto& children = node->getChildren();
    shared_ptr<ASTNode> body = nullptr;
    
    bool foundDo = false;
    for (const auto& child : children) {
        if (!child) continue;
        
        if (child->getType() == "KEYWORD" && child->getValue() == "do") {
            foundDo = true;
            continue;
        }
        
        if (foundDo && !body) {
            if (child->getType() != "KEYWORD" && child->getType() != "expression" && 
                child->getType() != "IDENTIFIER" && child->getType() != "ASSIGN_OPERATOR") {
                body = convertStatement(child);
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
    }
    else if (simpleExprs.size() == 2) {
        // Binary operation with relational operator
        auto left = convertSimpleExpression(simpleExprs[0]);
        auto right = convertSimpleExpression(simpleExprs[1]);
        
        // Find relational operator
        string op = "";
        auto relOp = findChild(node, "relational-operator");
        if (relOp && !relOp->getChildren().empty()) {
            op = relOp->getChildren()[0]->getValue();
        }
        
        return make_shared<BinOpNode>(op, left, right);
    }
    
    return nullptr;
}

shared_ptr<ASTNode> ASTBuilder::convertSimpleExpression(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;
    
    // <simple-expression> → [sign] <term> {addop <term>}
    
    auto terms = findChildren(node, "term");
    
    if (terms.empty()) return nullptr;
    
    // Start with first term
    shared_ptr<ASTNode> result = convertTerm(terms[0]);
    
    // Find additive operators
    auto addOps = findChildren(node, "additive-operator");
    
    // Combine terms with operators
    for (size_t i = 1; i < terms.size() && i - 1 < addOps.size(); i++) {
        auto right = convertTerm(terms[i]);
        string op = "";
        
        if (!addOps[i - 1]->getChildren().empty()) {
            op = addOps[i - 1]->getChildren()[0]->getValue();
        }
        
        result = make_shared<BinOpNode>(op, result, right);
    }
    
    // Check for unary sign at the beginning
    for (const auto& child : node->getChildren()) {
        if (child && child->getType() == "ARITHMETIC_OPERATOR") {
            string sign = child->getValue();
            if (sign == "-") {
                result = make_shared<UnaryOpNode>("-", result);
                break;
            }
        }
    }
    
    return result;
}

shared_ptr<ASTNode> ASTBuilder::convertTerm(shared_ptr<ParseNode> node) {
    if (!node) return nullptr;
    
    // <term> → <factor> {mulop <factor>}
    
    auto factors = findChildren(node, "factor");
    
    if (factors.empty()) return nullptr;
    
    // Start with first factor
    shared_ptr<ASTNode> result = convertFactor(factors[0]);
    
    // Find multiplication operators
    auto mulOps = findChildren(node, "multiplication-operator");
    
    // Combine factors with operators
    for (size_t i = 1; i < factors.size() && i - 1 < mulOps.size(); i++) {
        auto right = convertFactor(factors[i]);
        string op = "";
        
        if (!mulOps[i - 1]->getChildren().empty()) {
            op = mulOps[i - 1]->getChildren()[0]->getValue();
        }
        
        result = make_shared<BinOpNode>(op, result, right);
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
            return make_shared<VarNode>(child->getValue());
        }
        else if (child->getType() == "NUMBER") {
            string numStr = child->getValue();
            if (numStr.find('.') != string::npos) {
                return make_shared<NumberNode>(stod(numStr));
            } else {
                return make_shared<NumberNode>(stoi(numStr));
            }
        }
        else if (child->getType() == "STRING_LITERAL") {
            return make_shared<StringNode>(child->getValue());
        }
        else if (child->getType() == "CHAR_LITERAL") {
            string charStr = child->getValue();
            if (!charStr.empty()) {
                return make_shared<CharNode>(charStr[0]);
            }
        }
        else if (child->getType() == "expression") {
            // Parenthesized expression
            return convertExpression(child);
        }
        else if (child->getType() == "KEYWORD" && child->getValue() == "not") {
            // Unary not operator
            auto operand = convertFactor(node);
            return make_shared<UnaryOpNode>("not", operand);
        }
        else if (child->getType() == "factor") {
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
        return make_shared<VarNode>(identifier->getValue());
    }
    
    return nullptr;
}
