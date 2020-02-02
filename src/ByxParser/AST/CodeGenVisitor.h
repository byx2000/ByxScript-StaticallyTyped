#pragma once

#include "ASTVisitor.h"
#include "../../Common/CodeSeg.h"

#include <map>

class ByxParser;

class CodeGenVisitor : public ASTVisitor
{
public:
	CodeGenVisitor(ByxParser& parser, const std::string& curFunctionName = "");
	CodeSeg getCodeSeg();

private:
	ByxParser& parser;
	CodeSeg codeSeg;

	bool inGlobleScope;
	std::string curFunctionName;

	void printCode(const std::vector<Instruction>& code);

	virtual void visit(ProgramNode& node) override;
	virtual void visit(FunctionDeclareNode& node) override;
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
};