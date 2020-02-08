#pragma once

#include "ASTVisitor.h"
#include "../../Common/CodeSeg.h"
#include "../FunctionInfo.h"

class ByxParser;

class FuncCodeGenVisitor : public ASTVisitor
{
public:
	FuncCodeGenVisitor(ByxParser& parser, const std::string& curFuncName = "");
	CodeSeg getCode() const;

private:
	ByxParser& parser;
	std::string curFuncName;
	CodeSeg codeSeg;
	std::vector<int> breakStmtIndex;
	std::vector<int> continueStmtIndex;
	bool inLoop;

	virtual void visit(IntegerNode& node) override;
	virtual void visit(DoubleNode& node) override;
	virtual void visit(IntDeclareNode& node) override;
	virtual void visit(DoubleDeclareNode& node) override;
	virtual void visit(CodeBlockNode& node) override;
	virtual void visit(VarNode& node) override;
	virtual void visit(VarAssignNode& node) override;
	virtual void visit(ReturnNode& node) override;
	virtual void visit(FunctionCallStmtNode& node) override;
	virtual void visit(FunctionCallExprNode& node) override;
	virtual void visit(IfNode& node) override;
	virtual void visit(BinaryOpNode& node) override;
	virtual void visit(WhileNode& node) override;
	virtual void visit(BreakNode& node) override;
	virtual void visit(ContinueNode& node) override;
	virtual void visit(UnaryOpNode& node) override;
	virtual void visit(ForNode& node) override;
};