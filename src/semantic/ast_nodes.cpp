#include "../include/ast_nodes.h"
#include <iostream>

using namespace std;

// ==================== UTILITY FUNCTIONS ====================

string dataTypeToString(DataType type) {
    switch (type) {
        case DataType::VOID:
            return "void";
        case DataType::INTEGER:
            return "integer";
        case DataType::REAL:
            return "real";
        case DataType::BOOLEAN:
            return "boolean";
        case DataType::CHAR:
            return "char";
        case DataType::STRING:
            return "string";
        case DataType::ARRAY:
            return "array";
        case DataType::RECORD:
            return "record";
        case DataType::UNKNOWN:
            return "unknown";
        default:
            return "undefined";
    }
}

string objectTypeToString(ObjectType type) {
    switch (type) {
        case ObjectType::CONSTANT:
            return "constant";
        case ObjectType::VARIABLE:
            return "variable";
        case ObjectType::TYPE_DEF:
            return "type";
        case ObjectType::PROCEDURE:
            return "procedure";
        case ObjectType::FUNCTION:
            return "function";
        case ObjectType::PARAMETER:
            return "parameter";
        case ObjectType::ARRAY_TYPE:
            return "array_type";
        case ObjectType::RECORD_TYPE:
            return "record_type";
        case ObjectType::PROGRAM:
            return "program";
        default:
            return "undefined";
    }
}

// Helper function untuk indentasi
static void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        cout << "  ";
    }
}

// ==================== BASE AST NODE ====================

ASTNode::ASTNode()
    : dataType(DataType::UNKNOWN),
      symbolTableIndex(-1),
      scopeLevel(0),
      lineNumber(0),
      columnNumber(0) {}

// ==================== PROGRAM NODE ====================

ProgramNode::ProgramNode(const string& programName) : name(programName) {
    dataType = DataType::VOID;
}

void ProgramNode::addDeclaration(shared_ptr<ASTNode> decl) {
    declarations.push_back(decl);
}

void ProgramNode::setCompoundStatement(shared_ptr<ASTNode> stmt) {
    compoundStatement = stmt;
}

void ProgramNode::accept(ASTVisitor* visitor) {
    visitor->visitProgram(this);
}

void ProgramNode::print(int indent) const {
    printIndent(indent);
    cout << "ProgramNode(name: '" << name << "'";
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    cout << ")" << endl;

    if (!declarations.empty()) {
        printIndent(indent + 1);
        cout << "Declarations:" << endl;
        for (const auto& decl : declarations) {
            decl->print(indent + 2);
        }
    }

    if (compoundStatement) {
        printIndent(indent + 1);
        cout << "Body:" << endl;
        compoundStatement->print(indent + 2);
    }
}

// ==================== VARIABLE DECLARATION NODE ====================

VarDeclNode::VarDeclNode(const string& varName, DataType type)
    : name(varName), varType(type), customTypeName(""), isParameter(false), isVarParameter(false) {
    dataType = type;
}

void VarDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitVarDecl(this);
}

void VarDeclNode::print(int indent) const {
    printIndent(indent);
    cout << "VarDecl(name: '" << name << "', type: " << dataTypeToString(varType);
    cout << ", tab_index: " << symbolTableIndex;
    cout << ", level: " << scopeLevel;
    if (isParameter) {
        cout << ", parameter";
        if (isVarParameter) {
            cout << ", var";
        }
    }
    cout << ")" << endl;
}

// ==================== CONSTANT DECLARATION NODE ====================

ConstDeclNode::ConstDeclNode(const string& constName, shared_ptr<ASTNode> val)
    : name(constName), value(val) {
    if (val) {
        dataType = val->getDataType();
    }
}

void ConstDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitConstDecl(this);
}

void ConstDeclNode::print(int indent) const {
    printIndent(indent);
    cout << "ConstDecl(name: '" << name << "', type: " << dataTypeToString(dataType);
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    cout << ")" << endl;
    if (value) {
        printIndent(indent + 1);
        cout << "Value:" << endl;
        value->print(indent + 2);
    }
}

// ==================== TYPE DECLARATION NODE ====================

TypeDeclNode::TypeDeclNode(const string& typeName, DataType kind, shared_ptr<ASTNode> typeDef)
    : name(typeName), typeKind(kind), typeDefinition(typeDef) {
    dataType = kind;
}

void TypeDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitTypeDecl(this);
}

void TypeDeclNode::print(int indent) const {
    printIndent(indent);
    cout << "TypeDecl(name: '" << name << "', kind: " << dataTypeToString(typeKind);
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    cout << ")" << endl;
    if (typeDefinition) {
        typeDefinition->print(indent + 1);
    }
}

// ==================== ARRAY TYPE NODE ====================

ArrayTypeNode::ArrayTypeNode(int low, int high, DataType elemType)
    : lowBound(low),
      highBound(high),
      elementType(elemType),
      nestedElementType(nullptr),
      arrayTableIndex(-1) {
    dataType = DataType::ARRAY;
}

ArrayTypeNode::ArrayTypeNode(int low, int high, shared_ptr<ArrayTypeNode> nestedElemType)
    : lowBound(low),
      highBound(high),
      elementType(DataType::ARRAY),
      nestedElementType(nestedElemType),
      arrayTableIndex(-1) {
    dataType = DataType::ARRAY;
}

void ArrayTypeNode::accept(ASTVisitor* visitor) {
    visitor->visitArrayType(this);
}

void ArrayTypeNode::print(int indent) const {
    printIndent(indent);
    if (nestedElementType) {
        cout << "ArrayType(range: [" << lowBound << ".." << highBound << "], element_type: ARRAY";
        if (arrayTableIndex >= 0) {
            cout << ", atab_index: " << arrayTableIndex;
        }
        cout << ")" << endl;
        nestedElementType->print(indent + 1);
    } else {
        cout << "ArrayType(range: [" << lowBound << ".." << highBound
             << "], element_type: " << dataTypeToString(elementType);
        if (arrayTableIndex >= 0) {
            cout << ", atab_index: " << arrayTableIndex;
        }
        cout << ")" << endl;
    }
}

// ==================== RECORD TYPE NODE ====================

RecordTypeNode::RecordTypeNode() : blockTableIndex(-1) {
    dataType = DataType::RECORD;
}

void RecordTypeNode::addField(shared_ptr<VarDeclNode> field) {
    fields.push_back(field);
}

void RecordTypeNode::accept(ASTVisitor* visitor) {
    visitor->visitRecordType(this);
}

void RecordTypeNode::print(int indent) const {
    printIndent(indent);
    cout << "RecordType(";
    if (blockTableIndex >= 0) {
        cout << "btab_index: " << blockTableIndex;
    }
    cout << ")" << endl;

    if (!fields.empty()) {
        printIndent(indent + 1);
        cout << "Fields:" << endl;
        for (const auto& field : fields) {
            field->print(indent + 2);
        }
    }
}

// ==================== PROCEDURE DECLARATION NODE ====================

ProcedureDeclNode::ProcedureDeclNode(const string& procName) : name(procName), blockTableIndex(-1) {
    dataType = DataType::VOID;
}

void ProcedureDeclNode::addParameter(shared_ptr<VarDeclNode> param) {
    parameters.push_back(param);
}

void ProcedureDeclNode::addDeclaration(shared_ptr<ASTNode> decl) {
    declarations.push_back(decl);
}

void ProcedureDeclNode::setCompoundStatement(shared_ptr<ASTNode> stmt) {
    compoundStatement = stmt;
}

void ProcedureDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitProcedureDecl(this);
}

void ProcedureDeclNode::print(int indent) const {
    printIndent(indent);
    cout << "ProcedureDecl(name: '" << name << "'";
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    if (blockTableIndex >= 0) {
        cout << ", btab_index: " << blockTableIndex;
    }
    cout << ")" << endl;

    if (!parameters.empty()) {
        printIndent(indent + 1);
        cout << "Parameters:" << endl;
        for (const auto& param : parameters) {
            param->print(indent + 2);
        }
    }

    if (!declarations.empty()) {
        printIndent(indent + 1);
        cout << "Declarations:" << endl;
        for (const auto& decl : declarations) {
            decl->print(indent + 2);
        }
    }

    if (compoundStatement) {
        printIndent(indent + 1);
        cout << "Body:" << endl;
        compoundStatement->print(indent + 2);
    }
}

// ==================== FUNCTION DECLARATION NODE ====================

FunctionDeclNode::FunctionDeclNode(const string& funcName, DataType retType)
    : name(funcName), returnType(retType), blockTableIndex(-1) {
    dataType = retType;
}

void FunctionDeclNode::addParameter(shared_ptr<VarDeclNode> param) {
    parameters.push_back(param);
}

void FunctionDeclNode::addDeclaration(shared_ptr<ASTNode> decl) {
    declarations.push_back(decl);
}

void FunctionDeclNode::setCompoundStatement(shared_ptr<ASTNode> stmt) {
    compoundStatement = stmt;
}

void FunctionDeclNode::accept(ASTVisitor* visitor) {
    visitor->visitFunctionDecl(this);
}

void FunctionDeclNode::print(int indent) const {
    printIndent(indent);
    cout << "FunctionDecl(name: '" << name << "', return_type: " << dataTypeToString(returnType);
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    if (blockTableIndex >= 0) {
        cout << ", btab_index: " << blockTableIndex;
    }
    cout << ")" << endl;

    if (!parameters.empty()) {
        printIndent(indent + 1);
        cout << "Parameters:" << endl;
        for (const auto& param : parameters) {
            param->print(indent + 2);
        }
    }

    if (!declarations.empty()) {
        printIndent(indent + 1);
        cout << "Declarations:" << endl;
        for (const auto& decl : declarations) {
            decl->print(indent + 2);
        }
    }

    if (compoundStatement) {
        printIndent(indent + 1);
        cout << "Body:" << endl;
        compoundStatement->print(indent + 2);
    }
}

// ==================== COMPOUND STATEMENT NODE ====================

CompoundStatementNode::CompoundStatementNode() {
    dataType = DataType::VOID;
}

void CompoundStatementNode::addStatement(shared_ptr<ASTNode> stmt) {
    statements.push_back(stmt);
}

void CompoundStatementNode::accept(ASTVisitor* visitor) {
    visitor->visitCompoundStatement(this);
}

void CompoundStatementNode::print(int indent) const {
    printIndent(indent);
    cout << "CompoundStatement" << endl;
    for (const auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}

// ==================== ASSIGNMENT NODE ====================

AssignNode::AssignNode(shared_ptr<ASTNode> lhs, shared_ptr<ASTNode> rhs) : target(lhs), value(rhs) {
    dataType = DataType::VOID;
}

void AssignNode::accept(ASTVisitor* visitor) {
    visitor->visitAssign(this);
}

void AssignNode::print(int indent) const {
    printIndent(indent);
    cout << "Assign" << endl;

    if (target) {
        printIndent(indent + 1);
        cout << "Target:" << endl;
        target->print(indent + 2);
    }

    if (value) {
        printIndent(indent + 1);
        cout << "Value:" << endl;
        value->print(indent + 2);
    }
}

// ==================== IF NODE ====================

IfNode::IfNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> thenStmt, shared_ptr<ASTNode> elseStmt)
    : condition(cond), thenStatement(thenStmt), elseStatement(elseStmt) {
    dataType = DataType::VOID;
}

void IfNode::accept(ASTVisitor* visitor) {
    visitor->visitIf(this);
}

void IfNode::print(int indent) const {
    printIndent(indent);
    cout << "If" << endl;

    if (condition) {
        printIndent(indent + 1);
        cout << "Condition:" << endl;
        condition->print(indent + 2);
    }

    if (thenStatement) {
        printIndent(indent + 1);
        cout << "Then:" << endl;
        thenStatement->print(indent + 2);
    }

    if (elseStatement) {
        printIndent(indent + 1);
        cout << "Else:" << endl;
        elseStatement->print(indent + 2);
    }
}

// ==================== WHILE NODE ====================

WhileNode::WhileNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> bodyStmt)
    : condition(cond), body(bodyStmt) {
    dataType = DataType::VOID;
}

void WhileNode::accept(ASTVisitor* visitor) {
    visitor->visitWhile(this);
}

void WhileNode::print(int indent) const {
    printIndent(indent);
    cout << "While" << endl;

    if (condition) {
        printIndent(indent + 1);
        cout << "Condition:" << endl;
        condition->print(indent + 2);
    }

    if (body) {
        printIndent(indent + 1);
        cout << "Body:" << endl;
        body->print(indent + 2);
    }
}

// ==================== FOR NODE ====================

ForNode::ForNode(const string& loopVar, shared_ptr<ASTNode> start, shared_ptr<ASTNode> end,
                 bool downto, shared_ptr<ASTNode> bodyStmt)
    : loopVariable(loopVar), startValue(start), endValue(end), isDownto(downto), body(bodyStmt) {
    dataType = DataType::VOID;
}

void ForNode::accept(ASTVisitor* visitor) {
    visitor->visitFor(this);
}

void ForNode::print(int indent) const {
    printIndent(indent);
    cout << "For(variable: '" << loopVariable << "', direction: " << (isDownto ? "downto" : "to")
         << ")" << endl;

    if (startValue) {
        printIndent(indent + 1);
        cout << "Start:" << endl;
        startValue->print(indent + 2);
    }

    if (endValue) {
        printIndent(indent + 1);
        cout << "End:" << endl;
        endValue->print(indent + 2);
    }

    if (body) {
        printIndent(indent + 1);
        cout << "Body:" << endl;
        body->print(indent + 2);
    }
}

// ==================== PROCEDURE CALL NODE ====================

ProcCallNode::ProcCallNode(const string& procName) : name(procName) {
    dataType = DataType::VOID;
}

void ProcCallNode::addArgument(shared_ptr<ASTNode> arg) {
    arguments.push_back(arg);
}

void ProcCallNode::accept(ASTVisitor* visitor) {
    visitor->visitProcCall(this);
}

void ProcCallNode::print(int indent) const {
    printIndent(indent);
    cout << "ProcCall(name: '" << name << "'";
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    cout << ")" << endl;

    if (!arguments.empty()) {
        printIndent(indent + 1);
        cout << "Arguments:" << endl;
        for (const auto& arg : arguments) {
            arg->print(indent + 2);
        }
    }
}

// ==================== BINARY OPERATION NODE ====================

BinOpNode::BinOpNode(const string& operation, shared_ptr<ASTNode> leftOperand,
                     shared_ptr<ASTNode> rightOperand)
    : op(operation), left(leftOperand), right(rightOperand) {
    // Type akan di-set oleh semantic analyzer
}

void BinOpNode::accept(ASTVisitor* visitor) {
    visitor->visitBinOp(this);
}

void BinOpNode::print(int indent) const {
    printIndent(indent);
    cout << "BinOp(op: '" << op << "', type: " << dataTypeToString(dataType) << ")" << endl;

    if (left) {
        printIndent(indent + 1);
        cout << "Left:" << endl;
        left->print(indent + 2);
    }

    if (right) {
        printIndent(indent + 1);
        cout << "Right:" << endl;
        right->print(indent + 2);
    }
}

// ==================== UNARY OPERATION NODE ====================

UnaryOpNode::UnaryOpNode(const string& operation, shared_ptr<ASTNode> operandNode)
    : op(operation), operand(operandNode) {
    // Type akan di-set oleh semantic analyzer
}

void UnaryOpNode::accept(ASTVisitor* visitor) {
    visitor->visitUnaryOp(this);
}

void UnaryOpNode::print(int indent) const {
    printIndent(indent);
    cout << "UnaryOp(op: '" << op << "', type: " << dataTypeToString(dataType) << ")" << endl;

    if (operand) {
        printIndent(indent + 1);
        cout << "Operand:" << endl;
        operand->print(indent + 2);
    }
}

// ==================== NUMBER NODE ====================

NumberNode::NumberNode(int value) : intValue(value), realValue(0.0), isReal(false) {
    dataType = DataType::INTEGER;
}

NumberNode::NumberNode(double value) : intValue(0), realValue(value), isReal(true) {
    dataType = DataType::REAL;
}

void NumberNode::accept(ASTVisitor* visitor) {
    visitor->visitNumber(this);
}

void NumberNode::print(int indent) const {
    printIndent(indent);
    cout << "Number(";
    if (isReal) {
        cout << "real: " << realValue;
    } else {
        cout << "int: " << intValue;
    }
    cout << ", type: " << dataTypeToString(dataType) << ")" << endl;
}

// ==================== STRING NODE ====================

StringNode::StringNode(const string& str) : value(str) {
    dataType = DataType::STRING;
}

void StringNode::accept(ASTVisitor* visitor) {
    visitor->visitString(this);
}

void StringNode::print(int indent) const {
    printIndent(indent);
    cout << "String(value: \"" << value << "\")" << endl;
}

// ==================== CHAR NODE ====================

CharNode::CharNode(char ch) : value(ch) {
    dataType = DataType::CHAR;
}

void CharNode::accept(ASTVisitor* visitor) {
    visitor->visitChar(this);
}

void CharNode::print(int indent) const {
    printIndent(indent);
    cout << "Char(value: '" << value << "')" << endl;
}

// ==================== BOOL NODE ====================

BoolNode::BoolNode(bool val) : value(val) {
    dataType = DataType::BOOLEAN;
}

void BoolNode::accept(ASTVisitor* visitor) {
    visitor->visitBool(this);
}

void BoolNode::print(int indent) const {
    printIndent(indent);
    cout << "Bool(value: " << (value ? "true" : "false") << ")" << endl;
}

// ==================== VARIABLE NODE ====================

VarNode::VarNode(const string& varName) : name(varName) {
    // Type akan di-set oleh semantic analyzer setelah lookup
}

void VarNode::accept(ASTVisitor* visitor) {
    visitor->visitVar(this);
}

void VarNode::print(int indent) const {
    printIndent(indent);
    cout << "Var(name: '" << name << "'";
    if (symbolTableIndex >= 0) {
        cout << ", tab_index: " << symbolTableIndex;
    }
    if (dataType != DataType::UNKNOWN) {
        cout << ", type: " << dataTypeToString(dataType);
    }
    if (scopeLevel > 0) {
        cout << ", level: " << scopeLevel;
    }
    cout << ")" << endl;
}

// ==================== ARRAY ACCESS NODE ====================

ArrayAccessNode::ArrayAccessNode(shared_ptr<ASTNode> arr, shared_ptr<ASTNode> idx)
    : arrayVar(arr), index(idx) {
    // Type akan di-set oleh semantic analyzer
}

void ArrayAccessNode::accept(ASTVisitor* visitor) {
    visitor->visitArrayAccess(this);
}

void ArrayAccessNode::print(int indent) const {
    printIndent(indent);
    cout << "ArrayAccess(type: " << dataTypeToString(dataType) << ")" << endl;

    printIndent(indent + 1);
    cout << "Array:" << endl;
    arrayVar->print(indent + 2);

    printIndent(indent + 1);
    cout << "Index:" << endl;
    index->print(indent + 2);
}

// ==================== RECORD ACCESS NODE ====================

RecordAccessNode::RecordAccessNode(shared_ptr<ASTNode> rec, const string& field)
    : recordVar(rec), fieldName(field) {
    // Type akan di-set oleh semantic analyzer
}

void RecordAccessNode::accept(ASTVisitor* visitor) {
    visitor->visitRecordAccess(this);
}

void RecordAccessNode::print(int indent) const {
    printIndent(indent);
    cout << "RecordAccess(field: '" << fieldName << "', type: " << dataTypeToString(dataType) << ")"
         << endl;

    printIndent(indent + 1);
    cout << "Record:" << endl;
    recordVar->print(indent + 2);
}
