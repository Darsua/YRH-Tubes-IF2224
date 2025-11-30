#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast_nodes.h"
#include "symbol_table.h"
#include <vector>
#include <string>
#include <memory>

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable& symbolTable;
    std::vector<std::string> errors;
    int currentLevel;
    int currentBlockIndex;
    
    void addError(const std::string& msg) {
        errors.push_back(msg);
    }

public:
    SemanticAnalyzer(SymbolTable& symTable);
    
    void analyze(std::shared_ptr<ASTNode> root);
    
    const std::vector<std::string>& getErrors() const {
        return errors;
    }
    
    // Visitor methods
    void visitProgram(ProgramNode* node) override;
    void visitVarDecl(VarDeclNode* node) override;
    void visitConstDecl(ConstDeclNode* node) override;
    void visitTypeDecl(TypeDeclNode* node) override;
    void visitArrayType(ArrayTypeNode* node) override;
    void visitRecordType(RecordTypeNode* node) override;
    void visitProcedureDecl(ProcedureDeclNode* node) override;
    void visitFunctionDecl(FunctionDeclNode* node) override;
    void visitCompoundStatement(CompoundStatementNode* node) override;
    void visitAssign(AssignNode* node) override;
    void visitIf(IfNode* node) override;
    void visitWhile(WhileNode* node) override;
    void visitFor(ForNode* node) override;
    void visitProcCall(ProcCallNode* node) override;
    void visitBinOp(BinOpNode* node) override;
    void visitUnaryOp(UnaryOpNode* node) override;
    void visitNumber(NumberNode* node) override;
    void visitString(StringNode* node) override;
    void visitChar(CharNode* node) override;
    void visitBool(BoolNode* node) override;
    void visitVar(VarNode* node) override;
    void visitArrayAccess(ArrayAccessNode* node) override;
    void visitRecordAccess(RecordAccessNode* node) override;
};

#endif
