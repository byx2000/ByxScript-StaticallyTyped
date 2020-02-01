#pragma once

#include "AST.h"

class ASTVisitor
{
public:
	virtual void visit(ProgramNode& node);
	virtual void visit(IntegerNode& node);
	virtual void visit(DoubleNode& node);
	virtual void visit(IntDeclareNode& node);
	virtual void visit(DoubleDeclareNode& node);
	virtual void visit(FunctionDeclareNode& node);
	virtual void visit(CodeBlockNode& node);
	virtual void visit(VarNode& node);
	virtual void visit(VarAssignNode& node);
	virtual void visit(ReturnNode& node);
	virtual void visit(FunctionCallStmtNode& node);
	virtual void visit(FunctionCallExprNode& node);
	virtual void visit(IfNode& node);
};
