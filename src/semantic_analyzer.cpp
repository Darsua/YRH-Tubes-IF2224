#include "include/semantic_analyzer.h"
#include <iostream>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symTable) 
    : symbolTable(symTable), currentLevel(0), currentBlockIndex(0) {}

void SemanticAnalyzer::analyze(shared_ptr<ASTNode> root) {
    if (!root) {
        addError("No AST to analyze");
        return;
    }
    
    // Traverse AST and decorate with type/symbol info
    root->accept(this);
}

// Stub implementations - traverse and annotate AST nodes
void SemanticAnalyzer::visitProgram(ProgramNode* node) {
    // Global block (btab[0]) already created in SymbolTable constructor
    currentBlockIndex = 0;  // Use existing global block
    currentLevel = 0;
    
    // Add program name to symbol table
    TabEntry progEntry;
    progEntry.identifier = node->getName();
    progEntry.obj = static_cast<int>(ObjectClass::RESERVED);
    progEntry.typ = 0;
    progEntry.ref = 0;
    progEntry.nrm = 1;
    progEntry.lev = 0;
    progEntry.adr = 0;
    progEntry.link = 0;
    
    int progIdx = symbolTable.addSymbol(progEntry);
    node->setSymbolTableIndex(progIdx);
    
    // Process declarations - add to symbol table at level 0
    for (auto& decl : node->getDeclarations()) {
        decl->accept(this);
    }
    
    // Count variables in global scope
    int varCount = 0;
    for (auto& decl : node->getDeclarations()) {
        if (dynamic_pointer_cast<VarDeclNode>(decl)) {
            varCount++;
        }
    }
    
    // Update global block (btab[0]) with last and vsze
    BtabEntry* block = symbolTable.getBlock(0);
    if (block) {
        // last should be last symbol added (after declarations)
        // vsze should be number of variables
        block->vsze = varCount;
        symbolTable.updateBlock(0, *block);
    }
    
    // Process compound statement (main block at level 1)
    if (node->getCompoundStatement()) {
        // Enter main block scope (creates btab[1])
        symbolTable.enterScope(0, 0);
        currentLevel = 1;
        
        node->getCompoundStatement()->accept(this);
        
        symbolTable.exitScope();
        currentLevel = 0;
    }
}

void SemanticAnalyzer::visitVarDecl(VarDeclNode* node) {
    // Add variable to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::VARIABLE);
    
    // Map DataType to TypeCode
    DataType varType = node->getVarType();
    if (varType == DataType::INTEGER) entry.typ = static_cast<int>(TypeCode::INTS);
    else if (varType == DataType::REAL) entry.typ = static_cast<int>(TypeCode::REALS);
    else if (varType == DataType::BOOLEAN) entry.typ = static_cast<int>(TypeCode::BOOLS);
    else if (varType == DataType::CHAR) entry.typ = static_cast<int>(TypeCode::CHARS);
    else entry.typ = static_cast<int>(TypeCode::NOTYP);
    
    entry.ref = 0;
    entry.nrm = 1;
    entry.lev = currentLevel;
    entry.adr = 0;
    entry.link = 0;
    
    int idx = symbolTable.addSymbol(entry);
    if (idx < 0) {
        addError("Variable '" + node->getName() + "' already declared");
    }
}

void SemanticAnalyzer::visitConstDecl(ConstDeclNode* node) {
    if (node && node->getValue()) {
        node->getValue()->accept(this);
    }
}

void SemanticAnalyzer::visitTypeDecl(TypeDeclNode* node) {
    if (!node) return;
    // Type declaration processed
}

void SemanticAnalyzer::visitArrayType(ArrayTypeNode* node) {
    // Array type processed
}

void SemanticAnalyzer::visitRecordType(RecordTypeNode* node) {
    for (auto& field : node->getFields()) {
        field->accept(this);
    }
}

void SemanticAnalyzer::visitProcedureDecl(ProcedureDeclNode* node) {
    if (!node) return;
    
    // Add procedure to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::PROCEDURE);
    entry.typ = static_cast<int>(TypeCode::NOTYP);
    entry.ref = 0;
    entry.nrm = 1;
    entry.lev = currentLevel;
    entry.adr = 0;
    entry.link = 0;
    
    symbolTable.addSymbol(entry);
    
    // Enter procedure scope
    int savedLevel = currentLevel;
    symbolTable.enterScope(0, 0);
    currentLevel++;
    
    // Add parameters to symbol table
    for (auto& param : node->getParameters()) {
        if (param) param->accept(this);
    }
    
    // Add local declarations to symbol table
    for (auto& decl : node->getDeclarations()) {
        if (decl) decl->accept(this);
    }
    
    // Process body
    if (node->getCompoundStatement()) {
        node->getCompoundStatement()->accept(this);
    }
    
    // Exit procedure scope
    symbolTable.exitScope();
    currentLevel = savedLevel;
}

void SemanticAnalyzer::visitFunctionDecl(FunctionDeclNode* node) {
    if (!node) return;
    
    // Add function to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::FUNCTION);
    
    // Map return type from DataType to TypeCode
    DataType retType = node->getReturnType();
    if (retType == DataType::INTEGER) entry.typ = static_cast<int>(TypeCode::INTS);
    else if (retType == DataType::REAL) entry.typ = static_cast<int>(TypeCode::REALS);
    else if (retType == DataType::BOOLEAN) entry.typ = static_cast<int>(TypeCode::BOOLS);
    else if (retType == DataType::CHAR) entry.typ = static_cast<int>(TypeCode::CHARS);
    else entry.typ = static_cast<int>(TypeCode::NOTYP);
    
    entry.ref = 0;
    entry.nrm = 1;
    entry.lev = currentLevel;
    entry.adr = 0;
    entry.link = 0;
    
    symbolTable.addSymbol(entry);
    
    // Enter function scope
    int savedLevel = currentLevel;
    symbolTable.enterScope(0, 0);
    currentLevel++;
    
    // Add parameters to symbol table
    for (auto& param : node->getParameters()) {
        if (param) param->accept(this);
    }
    
    // Add local declarations to symbol table
    for (auto& decl : node->getDeclarations()) {
        if (decl) decl->accept(this);
    }
    
    // Process body
    if (node->getCompoundStatement()) {
        node->getCompoundStatement()->accept(this);
    }
    
    // Exit function scope
    symbolTable.exitScope();
    currentLevel = savedLevel;
}

void SemanticAnalyzer::visitCompoundStatement(CompoundStatementNode* node) {
    if (!node) return;
    for (auto& stmt : node->getStatements()) {
        if (stmt) stmt->accept(this);
    }
}

void SemanticAnalyzer::visitAssign(AssignNode* node) {
    if (!node) return;
    if (node->getTarget()) node->getTarget()->accept(this);
    if (node->getValue()) node->getValue()->accept(this);
}

void SemanticAnalyzer::visitIf(IfNode* node) {
    if (!node) return;
    if (node->getCondition()) node->getCondition()->accept(this);
    if (node->getThenStatement()) node->getThenStatement()->accept(this);
    if (node->getElseStatement()) {
        node->getElseStatement()->accept(this);
    }
}

void SemanticAnalyzer::visitWhile(WhileNode* node) {
    if (!node) return;
    if (node->getCondition()) node->getCondition()->accept(this);
    if (node->getBody()) node->getBody()->accept(this);
}

void SemanticAnalyzer::visitFor(ForNode* node) {
    if (node->getStartValue()) {
        node->getStartValue()->accept(this);
    }
    if (node->getEndValue()) {
        node->getEndValue()->accept(this);
    }
    if (node->getBody()) {
        node->getBody()->accept(this);
    }
}

void SemanticAnalyzer::visitProcCall(ProcCallNode* node) {
    auto* symbol = symbolTable.lookupSymbol(node->getName());
    if (symbol) {
        // Symbol found - just mark it
        node->setSymbolTableIndex(0);  // Simplified
    }
    
    for (auto& arg : node->getArguments()) {
        arg->accept(this);
    }
}

void SemanticAnalyzer::visitBinOp(BinOpNode* node) {
    if (!node) return;
    if (node->getLeft()) node->getLeft()->accept(this);
    if (node->getRight()) node->getRight()->accept(this);
    
    auto leftType = node->getLeft() ? node->getLeft()->getDataType() : DataType::UNKNOWN;
    auto rightType = node->getRight() ? node->getRight()->getDataType() : DataType::UNKNOWN;
    
    // Infer result type
    if (leftType == DataType::REAL || rightType == DataType::REAL) {
        node->setDataType(DataType::REAL);
    } else if (leftType == DataType::INTEGER || rightType == DataType::INTEGER) {
        node->setDataType(DataType::INTEGER);
    } else {
        node->setDataType(leftType);
    }
}

void SemanticAnalyzer::visitUnaryOp(UnaryOpNode* node) {
    if (!node) return;
    if (node->getOperand()) {
        node->getOperand()->accept(this);
        node->setDataType(node->getOperand()->getDataType());
    }
}

void SemanticAnalyzer::visitNumber(NumberNode* node) {
    // Type already set
}

void SemanticAnalyzer::visitString(StringNode* node) {
    node->setDataType(DataType::STRING);
}

void SemanticAnalyzer::visitChar(CharNode* node) {
    node->setDataType(DataType::CHAR);
}

void SemanticAnalyzer::visitBool(BoolNode* node) {
    node->setDataType(DataType::BOOLEAN);
}

void SemanticAnalyzer::visitVar(VarNode* node) {
    auto* symbol = symbolTable.lookupSymbol(node->getName());
    if (!symbol) {
        addError("Variable '" + node->getName() + "' not declared");
        node->setDataType(DataType::UNKNOWN);
        return;
    }
    
    // Map typ to DataType
    DataType type;
    switch(symbol->typ) {
        case 1: type = DataType::INTEGER; break;
        case 2: type = DataType::REAL; break;
        case 3: type = DataType::BOOLEAN; break;
        case 4: type = DataType::CHAR; break;
        default: type = DataType::UNKNOWN; break;
    }
    
    node->setDataType(type);
    node->setSymbolTableIndex(0);  // Simplified
    node->setScopeLevel(symbol->lev);
}

void SemanticAnalyzer::visitArrayAccess(ArrayAccessNode* node) {
    if (!node) return;
    if (node->getArrayVar()) node->getArrayVar()->accept(this);
    if (node->getIndex()) node->getIndex()->accept(this);
    node->setDataType(DataType::INTEGER);
}

void SemanticAnalyzer::visitRecordAccess(RecordAccessNode* node) {
    if (!node) return;
    if (node->getRecordVar()) node->getRecordVar()->accept(this);
    node->setDataType(DataType::INTEGER);
}
