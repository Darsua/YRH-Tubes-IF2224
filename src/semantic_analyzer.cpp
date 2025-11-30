#include "include/semantic_analyzer.h"
#include <iostream>
#include <algorithm>

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
    progEntry.obj = static_cast<int>(ObjectClass::PROGRAM);
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
    
    // Calculate addresses for global variables
    calculateVariableAddresses(0);
    
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
    // Check for redeclaration in current scope
    TabEntry* existing = symbolTable.lookupSymbol(node->getName(), true);
    if (existing && existing->lev == currentLevel) {
        reportError("Variable '" + node->getName() + "' already declared in this scope", node->getLineNumber());
        return;
    }
    
    // Add variable to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::VARIABLE);
    
    // Check if variable uses a custom type
    string customType = node->getCustomTypeName();
    if (!customType.empty()) {
        // Lookup the custom type in symbol table
        TabEntry* typeEntry = symbolTable.lookupSymbol(customType, false);
        if (typeEntry && typeEntry->obj == static_cast<int>(ObjectClass::TYPE)) {
            // Copy type and ref from the type definition
            entry.typ = typeEntry->typ;
            entry.ref = typeEntry->ref;
        } else {
            // Type not found, mark as unknown
            entry.typ = static_cast<int>(TypeCode::NOTYP);
            entry.ref = 0;
            addError("Undefined type '" + customType + "'");
        }
    } else {
        // Map DataType to TypeCode for primitive types
        DataType varType = node->getVarType();
        if (varType == DataType::INTEGER) entry.typ = static_cast<int>(TypeCode::INTS);
        else if (varType == DataType::REAL) entry.typ = static_cast<int>(TypeCode::REALS);
        else if (varType == DataType::BOOLEAN) entry.typ = static_cast<int>(TypeCode::BOOLS);
        else if (varType == DataType::CHAR) entry.typ = static_cast<int>(TypeCode::CHARS);
        else entry.typ = static_cast<int>(TypeCode::NOTYP);
        
        entry.ref = 0;
    }
    
    // Set nrm based on parameter type
    if (node->getIsParameter()) {
        if (node->getIsVarParameter()) {
            entry.nrm = 0;  // VAR parameter (by reference)
        } else {
            entry.nrm = 1;  // Normal parameter (by value)
        }
    } else {
        entry.nrm = 1;  // Normal variable
    }
    
    entry.lev = currentLevel;
    entry.adr = 0;  // Will be calculated later
    entry.link = 0;  // Will be set by addSymbol
    
    int idx = symbolTable.addSymbol(entry);
    if (idx < 0) {
        addError("Variable '" + node->getName() + "' already declared");
    } else {
        // Decorate the AST node with symbol table information
        node->setSymbolTableIndex(idx);
        node->setScopeLevel(currentLevel);
    }
}

void SemanticAnalyzer::visitConstDecl(ConstDeclNode* node) {
    if (!node) return;
    
    // Add constant to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::CONSTANT);
    
    // Process the constant value first to determine type
    if (node->getValue()) {
        node->getValue()->accept(this);
        DataType constType = node->getValue()->getDataType();
        
        if (constType == DataType::INTEGER) entry.typ = static_cast<int>(TypeCode::INTS);
        else if (constType == DataType::REAL) entry.typ = static_cast<int>(TypeCode::REALS);
        else if (constType == DataType::BOOLEAN) entry.typ = static_cast<int>(TypeCode::BOOLS);
        else if (constType == DataType::CHAR) entry.typ = static_cast<int>(TypeCode::CHARS);
        else entry.typ = static_cast<int>(TypeCode::NOTYP);
    } else {
        entry.typ = static_cast<int>(TypeCode::NOTYP);
    }
    
    entry.ref = 0;
    entry.nrm = 1;
    entry.lev = currentLevel;
    
    // Calculate constant value for adr field
    int constantValue = 0;
    if (node->getValue()) {
        if (auto numNode = dynamic_pointer_cast<NumberNode>(node->getValue())) {
            if (!numNode->getIsReal()) {
                constantValue = numNode->getIntValue();
            } else {
                // For real constants, would need special handling
                // For now, store as 0 (requires separate constant pool)
                constantValue = 0;
            }
        } else if (auto charNode = dynamic_pointer_cast<CharNode>(node->getValue())) {
            constantValue = static_cast<int>(charNode->getValue());
        }
        // String constants need pointer to string pool (not implemented yet)
    }
    entry.adr = constantValue;
    entry.link = 0;
    
    int idx = symbolTable.addSymbol(entry);
    if (idx < 0) {
        addError("Constant '" + node->getName() + "' already declared");
    } else {
        // Decorate the AST node with symbol table information
        node->setSymbolTableIndex(idx);
        node->setScopeLevel(currentLevel);
    }
}

void SemanticAnalyzer::visitTypeDecl(TypeDeclNode* node) {
    if (!node) return;
    
    int atab_ref = 0;  // Will be set if this is an array type
    
    // Process the type definition first (to get ATAB index for arrays)
    if (node->getTypeDefinition()) {
        node->getTypeDefinition()->accept(this);
        
        // If it's an array type, get the ATAB index
        if (auto arrayType = dynamic_pointer_cast<ArrayTypeNode>(node->getTypeDefinition())) {
            atab_ref = arrayType->getArrayTableIndex();
        }
    }
    
    // Add type to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::TYPE);
    
    // Map DataType to TypeCode
    DataType typeKind = node->getTypeKind();
    if (typeKind == DataType::INTEGER) entry.typ = static_cast<int>(TypeCode::INTS);
    else if (typeKind == DataType::REAL) entry.typ = static_cast<int>(TypeCode::REALS);
    else if (typeKind == DataType::BOOLEAN) entry.typ = static_cast<int>(TypeCode::BOOLS);
    else if (typeKind == DataType::CHAR) entry.typ = static_cast<int>(TypeCode::CHARS);
    else if (typeKind == DataType::ARRAY) entry.typ = static_cast<int>(TypeCode::ARRAYS);
    else if (typeKind == DataType::RECORD) entry.typ = static_cast<int>(TypeCode::RECORDS);
    else entry.typ = static_cast<int>(TypeCode::NOTYP);
    
    entry.ref = atab_ref;  // Point to ATAB for arrays
    entry.nrm = 1;
    entry.lev = currentLevel;
    entry.adr = 0;
    entry.link = 0;
    
    int idx = symbolTable.addSymbol(entry);
    if (idx < 0) {
        addError("Type '" + node->getName() + "' already declared");
    } else {
        // Decorate the AST node with symbol table information
        node->setSymbolTableIndex(idx);
        node->setScopeLevel(currentLevel);
    }
}

void SemanticAnalyzer::visitArrayType(ArrayTypeNode* node) {
    if (!node) return;
    
    // Add array type information to ATAB
    AtabEntry arrayEntry;
    
    // xtyp: Index type (usually integer for Pascal-S)
    arrayEntry.xtyp = static_cast<int>(TypeCode::INTS);  // Index type is INTEGER
    
    // Check if this is a multi-dimensional array
    if (node->isMultiDimensional()) {
        // Process nested array first (bottom-up approach)
        auto nestedArray = node->getNestedElementType();
        nestedArray->accept(this);
        
        // Get the ATAB index of the nested array
        int nestedAtabIndex = nestedArray->getArrayTableIndex();
        
        // etyp: ARRAYS (5) for multi-dimensional arrays
        arrayEntry.etyp = static_cast<int>(TypeCode::ARRAYS);
        
        // eref: points to the nested array's ATAB entry
        arrayEntry.eref = nestedAtabIndex;
        
        // elsz: size of one element (the entire nested array)
        arrayEntry.elsz = symbolTable.getArray(nestedAtabIndex)->size;
    } else {
        // Simple array: element is a primitive type
        DataType elemType = node->getElementType();
        int elementRef = 0;
        int elementSize = 1;
        
        if (elemType == DataType::INTEGER) {
            arrayEntry.etyp = static_cast<int>(TypeCode::INTS);
            elementSize = 1;
        }
        else if (elemType == DataType::REAL) {
            arrayEntry.etyp = static_cast<int>(TypeCode::REALS);
            elementSize = 1;
        }
        else if (elemType == DataType::BOOLEAN) {
            arrayEntry.etyp = static_cast<int>(TypeCode::BOOLS);
            elementSize = 1;
        }
        else if (elemType == DataType::CHAR) {
            arrayEntry.etyp = static_cast<int>(TypeCode::CHARS);
            elementSize = 1;
        }
        else {
            arrayEntry.etyp = static_cast<int>(TypeCode::NOTYP);
            elementSize = 1;
        }
        
        arrayEntry.eref = elementRef;
        arrayEntry.elsz = elementSize;
    }
    
    arrayEntry.low = node->getLowBound();
    arrayEntry.high = node->getHighBound();
    arrayEntry.size = (node->getHighBound() - node->getLowBound() + 1) * arrayEntry.elsz;
    
    int atab_idx = symbolTable.addArray(arrayEntry);
    node->setArrayTableIndex(atab_idx);
    node->setScopeLevel(currentLevel);
}

void SemanticAnalyzer::visitRecordType(RecordTypeNode* node) {
    if (!node) return;
    
    // Set scope level for record type
    node->setScopeLevel(currentLevel);
    
    // Process all fields
    for (auto& field : node->getFields()) {
        if (field) {
            field->accept(this);
        }
    }
}

void SemanticAnalyzer::visitProcedureDecl(ProcedureDeclNode* node) {
    if (!node) return;
    
    // Enter procedure scope first to get BTAB index
    int savedLevel = currentLevel;
    symbolTable.enterScope(0, 0);
    currentLevel++;
    int procBlockIdx = symbolTable.getCurrentBlock();
    
    // Add procedure to symbol table
    TabEntry entry;
    entry.identifier = node->getName();
    entry.obj = static_cast<int>(ObjectClass::PROCEDURE);
    entry.typ = static_cast<int>(TypeCode::NOTYP);
    entry.ref = procBlockIdx;  // Point to BTAB index
    entry.nrm = 1;
    entry.lev = savedLevel;  // Use saved level (declared at parent level)
    entry.adr = procBlockIdx;  // Address points to BTAB index
    entry.link = 0;
    
    // Add to parent scope, not current scope
    currentLevel = savedLevel;
    int idx = symbolTable.addSymbol(entry);
    currentLevel = savedLevel + 1;
    
    if (idx < 0) {
        addError("Procedure '" + node->getName() + "' already declared");
    } else {
        // Decorate the AST node with symbol table information
        node->setSymbolTableIndex(idx);
        node->setScopeLevel(savedLevel);
    }
    
    // Add parameters to symbol table
    for (auto& param : node->getParameters()) {
        if (param) param->accept(this);
    }
    
    // Add local declarations to symbol table
    for (auto& decl : node->getDeclarations()) {
        if (decl) decl->accept(this);
    }
    
    // Calculate addresses for parameters and local variables
    calculateParameterAddresses(procBlockIdx, node->getParameters().size());
    calculateVariableAddresses(procBlockIdx);
    
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
    
    // Enter function scope first to get BTAB index
    int savedLevel = currentLevel;
    symbolTable.enterScope(0, 0);
    currentLevel++;
    int funcBlockIdx = symbolTable.getCurrentBlock();
    
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
    
    entry.ref = funcBlockIdx;  // Point to BTAB index
    entry.nrm = 1;
    entry.lev = savedLevel;  // Use saved level (declared at parent level)
    entry.adr = funcBlockIdx;  // Address points to BTAB index
    entry.link = 0;
    
    // Add to parent scope, not current scope
    currentLevel = savedLevel;
    int idx = symbolTable.addSymbol(entry);
    currentLevel = savedLevel + 1;
    
    if (idx < 0) {
        addError("Function '" + node->getName() + "' already declared");
    } else {
        // Decorate the AST node with symbol table information
        node->setSymbolTableIndex(idx);
        node->setScopeLevel(savedLevel);
        node->setDataType(retType);
    }
    
    // Add parameters to symbol table
    for (auto& param : node->getParameters()) {
        if (param) param->accept(this);
    }
    
    // Add local declarations to symbol table
    for (auto& decl : node->getDeclarations()) {
        if (decl) decl->accept(this);
    }
    
    // Calculate addresses for parameters and local variables
    calculateParameterAddresses(funcBlockIdx, node->getParameters().size());
    calculateVariableAddresses(funcBlockIdx);
    
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
    if (!node) return;
    
    auto* symbol = symbolTable.lookupSymbol(node->getName());
    if (!symbol) {
        addError("Procedure/Function '" + node->getName() + "' not declared");
        node->setSymbolTableIndex(-1);
        node->setScopeLevel(currentLevel);
    } else {
        // Check if it's actually a procedure or function
        if (symbol->obj != static_cast<int>(ObjectClass::PROCEDURE) && 
            symbol->obj != static_cast<int>(ObjectClass::FUNCTION)) {
            addError("'" + node->getName() + "' is not a procedure or function");
        }
        
        // Decorate the AST node with actual symbol table index
        // We need to find the actual index in the table
        int symIdx = -1;
        for (size_t i = 0; i < symbolTable.getTabSize(); i++) {
            const TabEntry* entry = symbolTable.getSymbol(i);
            if (entry && entry->identifier == node->getName()) {
                symIdx = i;
                break;
            }
        }
        
        node->setSymbolTableIndex(symIdx);
        node->setScopeLevel(symbol->lev);
    }
    
    // Process arguments
    for (auto& arg : node->getArguments()) {
        if (arg) arg->accept(this);
    }
}

void SemanticAnalyzer::visitBinOp(BinOpNode* node) {
    if (!node) return;
    if (node->getLeft()) node->getLeft()->accept(this);
    if (node->getRight()) node->getRight()->accept(this);
    
    auto leftType = node->getLeft() ? node->getLeft()->getDataType() : DataType::UNKNOWN;
    auto rightType = node->getRight() ? node->getRight()->getDataType() : DataType::UNKNOWN;
    string op = node->getOp();
    
    // Type checking for binary operations
    // Arithmetic operators require numeric types
    if (op == "+" || op == "-" || op == "*" || op == "bagi" || op == "div" || op == "mod") {
        if (leftType != DataType::INTEGER && leftType != DataType::REAL && leftType != DataType::UNKNOWN) {
            reportError("Arithmetic operator '" + op + "' requires numeric left operand");
        }
        if (rightType != DataType::INTEGER && rightType != DataType::REAL && rightType != DataType::UNKNOWN) {
            reportError("Arithmetic operator '" + op + "' requires numeric right operand");
        }
        // Infer result type
        if (leftType == DataType::REAL || rightType == DataType::REAL) {
            node->setDataType(DataType::REAL);
        } else {
            node->setDataType(DataType::INTEGER);
        }
    }
    // Relational operators
    else if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "=" || op == "<>") {
        // Operands should be compatible
        node->setDataType(DataType::BOOLEAN);
    }
    // Logical operators
    else if (op == "dan" || op == "atau") {
        if (leftType != DataType::BOOLEAN && leftType != DataType::UNKNOWN) {
            reportError("Logical operator '" + op + "' requires boolean left operand");
        }
        if (rightType != DataType::BOOLEAN && rightType != DataType::UNKNOWN) {
            reportError("Logical operator '" + op + "' requires boolean right operand");
        }
        node->setDataType(DataType::BOOLEAN);
    }
    else {
        // Default fallback
        if (leftType == DataType::REAL || rightType == DataType::REAL) {
            node->setDataType(DataType::REAL);
        } else if (leftType == DataType::INTEGER || rightType == DataType::INTEGER) {
            node->setDataType(DataType::INTEGER);
        } else {
            node->setDataType(leftType);
        }
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
    if (!node) return;
    
    auto* symbol = symbolTable.lookupSymbol(node->getName());
    if (!symbol) {
        addError("Variable '" + node->getName() + "' not declared");
        node->setDataType(DataType::UNKNOWN);
        node->setSymbolTableIndex(-1);
        node->setScopeLevel(currentLevel);
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
    
    // Find the actual index in the symbol table
    int symIdx = -1;
    for (size_t i = 0; i < symbolTable.getTabSize(); i++) {
        const TabEntry* entry = symbolTable.getSymbol(i);
        if (entry && entry->identifier == node->getName()) {
            symIdx = i;
            break;
        }
    }
    
    node->setDataType(type);
    node->setSymbolTableIndex(symIdx);
    node->setScopeLevel(symbol->lev);
}

void SemanticAnalyzer::visitArrayAccess(ArrayAccessNode* node) {
    if (!node) return;
    
    // Process the array variable
    if (node->getArrayVar()) {
        node->getArrayVar()->accept(this);
        
        // Inherit scope level from array variable
        node->setScopeLevel(node->getArrayVar()->getScopeLevel());
        node->setSymbolTableIndex(node->getArrayVar()->getSymbolTableIndex());
    } else {
        node->setScopeLevel(currentLevel);
        node->setSymbolTableIndex(-1);
    }
    
    // Process the index expression
    if (node->getIndex()) {
        node->getIndex()->accept(this);
    }
    
    // For now, set element type to INTEGER (would need proper type lookup from ATAB)
    node->setDataType(DataType::INTEGER);
}

void SemanticAnalyzer::visitRecordAccess(RecordAccessNode* node) {
    if (!node) return;
    
    // Process the record variable
    if (node->getRecordVar()) {
        node->getRecordVar()->accept(this);
        
        // Inherit scope level from record variable
        node->setScopeLevel(node->getRecordVar()->getScopeLevel());
        node->setSymbolTableIndex(node->getRecordVar()->getSymbolTableIndex());
    } else {
        node->setScopeLevel(currentLevel);
        node->setSymbolTableIndex(-1);
    }
    
    // For now, set field type to INTEGER (would need proper field lookup)
    node->setDataType(DataType::INTEGER);
}

// ==================== ADDRESS CALCULATION HELPERS ====================

void SemanticAnalyzer::calculateParameterAddresses(int blockIdx, size_t paramCount) {
    if (blockIdx < 0 || blockIdx >= symbolTable.getBtabSize()) return;
    
    BtabEntry* block = symbolTable.getBlock(blockIdx);
    if (!block) return;
    
    // Collect parameter symbols (they are the first N symbols in the block)
    vector<int> paramIndices;
    int idx = block->last;
    
    // Traverse backward to find all symbols
    while (idx > 0 && paramIndices.size() < paramCount * 10) {  // Safety limit
        TabEntry* entry = symbolTable.getSymbol(idx);
        if (!entry) break;
        
        if (entry->obj == static_cast<int>(ObjectClass::VARIABLE) && 
            entry->lev == currentLevel) {
            paramIndices.push_back(idx);
        }
        
        if (idx == entry->link) break;  // Prevent infinite loop
        idx = entry->link;
    }
    
    // Reverse to get declaration order
    reverse(paramIndices.begin(), paramIndices.end());
    
    // Calculate parameter addresses (first paramCount symbols are parameters)
    int paramOffset = 0;
    int paramSize = 0;
    int lastParamIndex = 0;
    for (size_t i = 0; i < paramCount && i < paramIndices.size(); i++) {
        TabEntry* entry = symbolTable.getSymbol(paramIndices[i]);
        if (!entry) continue;
        
        int size = 1;  // Default parameter size
        
        // Arrays passed by reference use 1 word (pointer)
        if (entry->typ == static_cast<int>(TypeCode::ARRAYS)) {
            size = 1;
        }
        
        entry->adr = paramOffset;
        symbolTable.updateSymbol(paramIndices[i], *entry);
        
        paramOffset += size;
        paramSize += size;
        
        // Track the last parameter index
        lastParamIndex = paramIndices[i];
    }
    
    // Update block parameter size and lpar
    block->psze = paramSize;
    block->lpar = (paramCount > 0) ? lastParamIndex : 0;
    symbolTable.updateBlock(blockIdx, *block);
}

void SemanticAnalyzer::calculateVariableAddresses(int blockIdx) {
    if (blockIdx < 0 || blockIdx >= symbolTable.getBtabSize()) return;
    
    BtabEntry* block = symbolTable.getBlock(blockIdx);
    if (!block) return;
    
    // Collect all variable symbols in this block
    vector<int> varIndices;
    int idx = block->last;
    int expectedLevel = (blockIdx == 0) ? 0 : currentLevel;
    
    while (idx > 0) {
        TabEntry* entry = symbolTable.getSymbol(idx);
        if (!entry) break;
        
        if (entry->lev != expectedLevel) break;  // Different level
        
        // Only process non-parameter variables
        if (entry->obj == static_cast<int>(ObjectClass::VARIABLE)) {
            varIndices.push_back(idx);
        }
        
        if (idx == entry->link) break;  // Prevent infinite loop
        idx = entry->link;
    }
    
    // Reverse to get declaration order
    reverse(varIndices.begin(), varIndices.end());
    
    // Skip parameters (they already have addresses set)
    // Parameters have lower indices (added first)
    int varOffset = 0;
    int varSize = 0;
    
    for (int varIdx : varIndices) {
        TabEntry* entry = symbolTable.getSymbol(varIdx);
        if (!entry) continue;
        
        // Skip if address already set (it's a parameter)
        if (entry->adr > 0 || (varOffset == 0 && entry->adr == 0)) {
            // For parameters, skip if part of block->psze
            if (block->psze > 0 && varSize < static_cast<size_t>(block->psze)) {
                varSize++;
                continue;
            }
        }
        
        int size = 1;  // Default variable size
        
        // Calculate size for arrays
        if (entry->typ == static_cast<int>(TypeCode::ARRAYS)) {
            AtabEntry* arrayInfo = symbolTable.getArray(entry->ref);
            if (arrayInfo) {
                size = arrayInfo->size;
            }
        }
        
        entry->adr = varOffset;
        symbolTable.updateSymbol(varIdx, *entry);
        
        varOffset += size;
        varSize += size;
    }
    
    // Update block variable size
    block->vsze = varOffset;
    symbolTable.updateBlock(blockIdx, *block);
}
