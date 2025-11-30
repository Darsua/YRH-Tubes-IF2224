#ifndef AST_NODES_H
#define AST_NODES_H

#include <string>
#include <vector>
#include <memory>

using namespace std;
class ASTVisitor;

enum class DataType {
    VOID,
    INTEGER,
    REAL,
    BOOLEAN,
    CHAR,
    STRING,
    ARRAY,
    RECORD,
    UNKNOWN
};

enum class ObjectType {
    CONSTANT,
    VARIABLE,
    TYPE_DEF,
    PROCEDURE,
    FUNCTION,
    PARAMETER,
    ARRAY_TYPE,
    RECORD_TYPE,
    PROGRAM
};

class ASTNode {
protected:
    DataType dataType;      // Tipe data dari node
    int symbolTableIndex;   // Index di symbol table (-1 jika tidak ada)
    int scopeLevel;         // Lexical level/scope
    int lineNumber;         // Line number untuk error reporting
    int columnNumber;       // Column number untuk error reporting

public:
    ASTNode();
    virtual ~ASTNode() = default;
    
    // Getters
    DataType getDataType() const { return dataType; }
    int getSymbolTableIndex() const { return symbolTableIndex; }
    int getScopeLevel() const { return scopeLevel; }
    int getLineNumber() const { return lineNumber; }
    int getColumnNumber() const { return columnNumber; }
    
    // Setters untuk semantic analysis
    void setDataType(DataType type) { dataType = type; }
    void setSymbolTableIndex(int index) { symbolTableIndex = index; }
    void setScopeLevel(int level) { scopeLevel = level; }
    void setLineNumber(int line) { lineNumber = line; }
    void setColumnNumber(int column) { columnNumber = column; }
    
    // Virtual method untuk visitor pattern
    virtual void accept(ASTVisitor* visitor) = 0;
    
    // Virtual method untuk printing AST
    virtual void print(int indent = 0) const = 0;
};

// ==================== PROGRAM NODE ====================
class ProgramNode : public ASTNode {
private:
    string name;
    vector<shared_ptr<ASTNode>> declarations;
    shared_ptr<ASTNode> compoundStatement;

public:
    ProgramNode(const string& programName);
    
    void addDeclaration(shared_ptr<ASTNode> decl);
    void setCompoundStatement(shared_ptr<ASTNode> stmt);
    
    string getName() const { return name; }
    const vector<shared_ptr<ASTNode>>& getDeclarations() const { return declarations; }
    shared_ptr<ASTNode> getCompoundStatement() const { return compoundStatement; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// ==================== DECLARATION NODES ====================

// Variable Declaration Node
class VarDeclNode : public ASTNode {
private:
    string name;
    DataType varType;
    string customTypeName;  // For custom types (e.g., "DaftarNilai")
    bool isParameter;       // true jika ini adalah parameter
    bool isVarParameter;    // true jika parameter by reference (var)

public:
    VarDeclNode(const string& varName, DataType type);
    
    string getName() const { return name; }
    DataType getVarType() const { return varType; }
    string getCustomTypeName() const { return customTypeName; }
    bool getIsParameter() const { return isParameter; }
    bool getIsVarParameter() const { return isVarParameter; }
    
    void setCustomTypeName(const string& typeName) { customTypeName = typeName; }
    void setIsParameter(bool param) { isParameter = param; }
    void setIsVarParameter(bool varParam) { isVarParameter = varParam; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Constant Declaration Node
class ConstDeclNode : public ASTNode {
private:
    string name;
    shared_ptr<ASTNode> value;  // Bisa NumberNode, StringNode, BoolNode

public:
    ConstDeclNode(const string& constName, shared_ptr<ASTNode> val);
    
    string getName() const { return name; }
    shared_ptr<ASTNode> getValue() const { return value; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Type Declaration Node (untuk array dan record)
class TypeDeclNode : public ASTNode {
private:
    string name;
    DataType typeKind;      // ARRAY atau RECORD
    shared_ptr<ASTNode> typeDefinition;  // ArrayTypeNode atau RecordTypeNode

public:
    TypeDeclNode(const string& typeName, DataType kind, shared_ptr<ASTNode> typeDef);
    
    string getName() const { return name; }
    DataType getTypeKind() const { return typeKind; }
    shared_ptr<ASTNode> getTypeDefinition() const { return typeDefinition; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Array Type Node
class ArrayTypeNode : public ASTNode {
private:
    int lowBound;
    int highBound;
    DataType elementType;
    shared_ptr<ArrayTypeNode> nestedElementType; // For multi-dimensional arrays
    int arrayTableIndex;    // Index di atab

public:
    ArrayTypeNode(int low, int high, DataType elemType);
    ArrayTypeNode(int low, int high, shared_ptr<ArrayTypeNode> nestedElemType);
    
    int getLowBound() const { return lowBound; }
    int getHighBound() const { return highBound; }
    DataType getElementType() const { return elementType; }
    shared_ptr<ArrayTypeNode> getNestedElementType() const { return nestedElementType; }
    bool isMultiDimensional() const { return nestedElementType != nullptr; }
    int getArrayTableIndex() const { return arrayTableIndex; }
    
    void setArrayTableIndex(int index) { arrayTableIndex = index; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Record Type Node
class RecordTypeNode : public ASTNode {
private:
    vector<shared_ptr<VarDeclNode>> fields;
    int blockTableIndex;    // Index di btab

public:
    RecordTypeNode();
    
    void addField(shared_ptr<VarDeclNode> field);
    const vector<shared_ptr<VarDeclNode>>& getFields() const { return fields; }
    int getBlockTableIndex() const { return blockTableIndex; }
    
    void setBlockTableIndex(int index) { blockTableIndex = index; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Procedure Declaration Node
class ProcedureDeclNode : public ASTNode {
private:
    string name;
    vector<shared_ptr<VarDeclNode>> parameters;
    vector<shared_ptr<ASTNode>> declarations;
    shared_ptr<ASTNode> compoundStatement;
    int blockTableIndex;    // Index di btab

public:
    ProcedureDeclNode(const string& procName);
    
    void addParameter(shared_ptr<VarDeclNode> param);
    void addDeclaration(shared_ptr<ASTNode> decl);
    void setCompoundStatement(shared_ptr<ASTNode> stmt);
    
    string getName() const { return name; }
    const vector<shared_ptr<VarDeclNode>>& getParameters() const { return parameters; }
    const vector<shared_ptr<ASTNode>>& getDeclarations() const { return declarations; }
    shared_ptr<ASTNode> getCompoundStatement() const { return compoundStatement; }
    int getBlockTableIndex() const { return blockTableIndex; }
    
    void setBlockTableIndex(int index) { blockTableIndex = index; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Function Declaration Node
class FunctionDeclNode : public ASTNode {
private:
    string name;
    DataType returnType;
    vector<shared_ptr<VarDeclNode>> parameters;
    vector<shared_ptr<ASTNode>> declarations;
    shared_ptr<ASTNode> compoundStatement;
    int blockTableIndex;    // Index di btab

public:
    FunctionDeclNode(const string& funcName, DataType retType);
    
    void addParameter(shared_ptr<VarDeclNode> param);
    void addDeclaration(shared_ptr<ASTNode> decl);
    void setCompoundStatement(shared_ptr<ASTNode> stmt);
    
    string getName() const { return name; }
    DataType getReturnType() const { return returnType; }
    const vector<shared_ptr<VarDeclNode>>& getParameters() const { return parameters; }
    const vector<shared_ptr<ASTNode>>& getDeclarations() const { return declarations; }
    shared_ptr<ASTNode> getCompoundStatement() const { return compoundStatement; }
    int getBlockTableIndex() const { return blockTableIndex; }
    
    void setBlockTableIndex(int index) { blockTableIndex = index; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// ==================== STATEMENT NODES ====================

// Compound Statement Node (begin...end)
class CompoundStatementNode : public ASTNode {
private:
    vector<shared_ptr<ASTNode>> statements;

public:
    CompoundStatementNode();
    
    void addStatement(shared_ptr<ASTNode> stmt);
    const vector<shared_ptr<ASTNode>>& getStatements() const { return statements; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Assignment Statement Node
class AssignNode : public ASTNode {
private:
    shared_ptr<ASTNode> target;     // VarNode atau ArrayAccessNode atau RecordAccessNode
    shared_ptr<ASTNode> value;      // Expression

public:
    AssignNode(shared_ptr<ASTNode> lhs, shared_ptr<ASTNode> rhs);
    
    shared_ptr<ASTNode> getTarget() const { return target; }
    shared_ptr<ASTNode> getValue() const { return value; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// If Statement Node
class IfNode : public ASTNode {
private:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> thenStatement;
    shared_ptr<ASTNode> elseStatement;  // nullptr jika tidak ada else

public:
    IfNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> thenStmt, shared_ptr<ASTNode> elseStmt = nullptr);
    
    shared_ptr<ASTNode> getCondition() const { return condition; }
    shared_ptr<ASTNode> getThenStatement() const { return thenStatement; }
    shared_ptr<ASTNode> getElseStatement() const { return elseStatement; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// While Statement Node
class WhileNode : public ASTNode {
private:
    shared_ptr<ASTNode> condition;
    shared_ptr<ASTNode> body;

public:
    WhileNode(shared_ptr<ASTNode> cond, shared_ptr<ASTNode> bodyStmt);
    
    shared_ptr<ASTNode> getCondition() const { return condition; }
    shared_ptr<ASTNode> getBody() const { return body; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// For Statement Node
class ForNode : public ASTNode {
private:
    string loopVariable;
    shared_ptr<ASTNode> startValue;
    shared_ptr<ASTNode> endValue;
    bool isDownto;          // true jika downto, false jika to
    shared_ptr<ASTNode> body;

public:
    ForNode(const string& loopVar, shared_ptr<ASTNode> start, shared_ptr<ASTNode> end, 
            bool downto, shared_ptr<ASTNode> bodyStmt);
    
    string getLoopVariable() const { return loopVariable; }
    shared_ptr<ASTNode> getStartValue() const { return startValue; }
    shared_ptr<ASTNode> getEndValue() const { return endValue; }
    bool getIsDownto() const { return isDownto; }
    shared_ptr<ASTNode> getBody() const { return body; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Procedure/Function Call Node
class ProcCallNode : public ASTNode {
private:
    string name;
    vector<shared_ptr<ASTNode>> arguments;

public:
    ProcCallNode(const string& procName);
    
    void addArgument(shared_ptr<ASTNode> arg);
    
    string getName() const { return name; }
    const vector<shared_ptr<ASTNode>>& getArguments() const { return arguments; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// ==================== EXPRESSION NODES ====================

// Binary Operation Node
class BinOpNode : public ASTNode {
private:
    string op;              // +, -, *, /, div, mod, and, or, =, <>, <, >, <=, >=
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;

public:
    BinOpNode(const string& operation, shared_ptr<ASTNode> leftOperand, shared_ptr<ASTNode> rightOperand);
    
    string getOp() const { return op; }
    shared_ptr<ASTNode> getLeft() const { return left; }
    shared_ptr<ASTNode> getRight() const { return right; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Unary Operation Node
class UnaryOpNode : public ASTNode {
private:
    string op;              // -, not
    shared_ptr<ASTNode> operand;

public:
    UnaryOpNode(const string& operation, shared_ptr<ASTNode> operandNode);
    
    string getOp() const { return op; }
    shared_ptr<ASTNode> getOperand() const { return operand; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Number Literal Node
class NumberNode : public ASTNode {
private:
    int intValue;
    double realValue;
    bool isReal;

public:
    NumberNode(int value);
    NumberNode(double value);
    
    int getIntValue() const { return intValue; }
    double getRealValue() const { return realValue; }
    bool getIsReal() const { return isReal; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// String Literal Node
class StringNode : public ASTNode {
private:
    string value;

public:
    StringNode(const string& str);
    
    string getValue() const { return value; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Char Literal Node
class CharNode : public ASTNode {
private:
    char value;

public:
    CharNode(char ch);
    
    char getValue() const { return value; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Boolean Literal Node
class BoolNode : public ASTNode {
private:
    bool value;

public:
    BoolNode(bool val);
    
    bool getValue() const { return value; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Variable Reference Node
class VarNode : public ASTNode {
private:
    string name;

public:
    VarNode(const string& varName);
    
    string getName() const { return name; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Array Access Node (arr[index])
class ArrayAccessNode : public ASTNode {
private:
    shared_ptr<ASTNode> arrayVar;   // VarNode atau nested ArrayAccessNode
    shared_ptr<ASTNode> index;      // Expression

public:
    ArrayAccessNode(shared_ptr<ASTNode> arr, shared_ptr<ASTNode> idx);
    
    shared_ptr<ASTNode> getArrayVar() const { return arrayVar; }
    shared_ptr<ASTNode> getIndex() const { return index; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// Record Access Node (record.field)
class RecordAccessNode : public ASTNode {
private:
    shared_ptr<ASTNode> recordVar;  // VarNode atau nested RecordAccessNode
    string fieldName;

public:
    RecordAccessNode(shared_ptr<ASTNode> rec, const string& field);
    
    shared_ptr<ASTNode> getRecordVar() const { return recordVar; }
    string getFieldName() const { return fieldName; }
    
    void accept(ASTVisitor* visitor) override;
    void print(int indent = 0) const override;
};

// ==================== VISITOR INTERFACE ====================

// Abstract Visitor class untuk visitor pattern
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Visit methods untuk setiap node type
    virtual void visitProgram(ProgramNode* node) = 0;
    virtual void visitVarDecl(VarDeclNode* node) = 0;
    virtual void visitConstDecl(ConstDeclNode* node) = 0;
    virtual void visitTypeDecl(TypeDeclNode* node) = 0;
    virtual void visitArrayType(ArrayTypeNode* node) = 0;
    virtual void visitRecordType(RecordTypeNode* node) = 0;
    virtual void visitProcedureDecl(ProcedureDeclNode* node) = 0;
    virtual void visitFunctionDecl(FunctionDeclNode* node) = 0;
    virtual void visitCompoundStatement(CompoundStatementNode* node) = 0;
    virtual void visitAssign(AssignNode* node) = 0;
    virtual void visitIf(IfNode* node) = 0;
    virtual void visitWhile(WhileNode* node) = 0;
    virtual void visitFor(ForNode* node) = 0;
    virtual void visitProcCall(ProcCallNode* node) = 0;
    virtual void visitBinOp(BinOpNode* node) = 0;
    virtual void visitUnaryOp(UnaryOpNode* node) = 0;
    virtual void visitNumber(NumberNode* node) = 0;
    virtual void visitString(StringNode* node) = 0;
    virtual void visitChar(CharNode* node) = 0;
    virtual void visitBool(BoolNode* node) = 0;
    virtual void visitVar(VarNode* node) = 0;
    virtual void visitArrayAccess(ArrayAccessNode* node) = 0;
    virtual void visitRecordAccess(RecordAccessNode* node) = 0;
};

// ==================== UTILITY FUNCTIONS ====================

// Helper function untuk convert DataType ke string
string dataTypeToString(DataType type);

// Helper function untuk convert ObjectType ke string
string objectTypeToString(ObjectType type);

#endif // AST_NODES_H
