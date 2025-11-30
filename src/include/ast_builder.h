#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "ast_nodes.h"
#include "parser.h"
#include <memory>
#include <vector>
#include <string>

using namespace std;

/**
 * AST Builder - Converts Parse Tree to Abstract Syntax Tree
 *
 * This class traverses the parse tree produced by the Parser and
 * constructs a simplified AST suitable for semantic analysis.
 */
class ASTBuilder {
   private:
    shared_ptr<ParseNode> parseTree;
    vector<string> errors;

    // Helper functions
    bool isTerminal(shared_ptr<ParseNode> node) const;
    string getTokenValue(shared_ptr<ParseNode> node) const;
    string getNodeType(shared_ptr<ParseNode> node) const;
    shared_ptr<ParseNode> findChild(shared_ptr<ParseNode> node, const string& type) const;
    vector<shared_ptr<ParseNode>> findChildren(shared_ptr<ParseNode> node,
                                               const string& type) const;
    void setPosition(shared_ptr<ASTNode> astNode, shared_ptr<ParseNode> parseNode);

    // Type conversion
    DataType stringToDataType(const string& typeStr);

    // Main conversion functions
    shared_ptr<ProgramNode> convertProgram(shared_ptr<ParseNode> node);

    // Declaration conversions
    vector<shared_ptr<ASTNode>> convertDeclarationPart(shared_ptr<ParseNode> node);
    vector<shared_ptr<VarDeclNode>> convertVarDeclaration(shared_ptr<ParseNode> node);
    vector<shared_ptr<ConstDeclNode>> convertConstDeclaration(shared_ptr<ParseNode> node);
    vector<shared_ptr<TypeDeclNode>> convertTypeDeclaration(shared_ptr<ParseNode> node);
    shared_ptr<ProcedureDeclNode> convertProcedureDeclaration(shared_ptr<ParseNode> node);
    shared_ptr<FunctionDeclNode> convertFunctionDeclaration(shared_ptr<ParseNode> node);

    // Type conversions
    DataType convertType(shared_ptr<ParseNode> node);
    shared_ptr<ArrayTypeNode> convertArrayType(shared_ptr<ParseNode> node);
    shared_ptr<RecordTypeNode> convertRecordType(shared_ptr<ParseNode> node);

    // Parameter conversions
    vector<shared_ptr<VarDeclNode>> convertFormalParameterList(shared_ptr<ParseNode> node);

    // Statement conversions
    shared_ptr<CompoundStatementNode> convertCompoundStatement(shared_ptr<ParseNode> node);
    vector<shared_ptr<ASTNode>> convertStatementList(shared_ptr<ParseNode> node);
    shared_ptr<ASTNode> convertStatement(shared_ptr<ParseNode> node);
    shared_ptr<AssignNode> convertAssignmentStatement(shared_ptr<ParseNode> node);
    shared_ptr<IfNode> convertIfStatement(shared_ptr<ParseNode> node);
    shared_ptr<WhileNode> convertWhileStatement(shared_ptr<ParseNode> node);
    shared_ptr<ForNode> convertForStatement(shared_ptr<ParseNode> node);
    shared_ptr<ProcCallNode> convertProcedureCall(shared_ptr<ParseNode> node);

    // Expression conversions
    shared_ptr<ASTNode> convertExpression(shared_ptr<ParseNode> node);
    shared_ptr<ASTNode> convertSimpleExpression(shared_ptr<ParseNode> node);
    shared_ptr<ASTNode> convertTerm(shared_ptr<ParseNode> node);
    shared_ptr<ASTNode> convertFactor(shared_ptr<ParseNode> node);

    // Helper for variable reference (could be array access or record access)
    shared_ptr<ASTNode> convertVariableAccess(shared_ptr<ParseNode> node);

    // Helper to extract number from nested expression tree
    int extractNumberFromExpression(shared_ptr<ParseNode> expr) const;

    // Error handling
    void error(const string& message);

   public:
    ASTBuilder();

    // Main build function
    shared_ptr<ProgramNode> buildAST(shared_ptr<ParseNode> parseTree);

    // Error handling
    bool hasErrors() const {
        return !errors.empty();
    }
    const vector<string>& getErrors() const {
        return errors;
    }
};

#endif  // AST_BUILDER_H
