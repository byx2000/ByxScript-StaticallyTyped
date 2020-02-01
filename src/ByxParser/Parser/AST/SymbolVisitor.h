#pragma once

#include "ASTVisitor.h"
#include "../Scope/ScopeStack.h"
#include "../../../Common/FunctionTable.h"

class SymbolVisitor : public ASTVisitor
{
public:
	SymbolVisitor();
	int getGlobalSpace();
	FunctionTable getFunctionTable();
	std::map<std::string, FunctionInfo> getFunctionInfo();

private:
	bool inGlobalScope; // 标记是否处于全局作用域
	int globalVarIndex; // 用于计算全局变量索引
	int funcIndex; // 用于计算函数索引
	ScopeStack scopeStack; // 局部作用域栈
	std::map<std::string, SymbolInfo> globalVarInfo; // 全局变量信息
	std::map<std::string, FunctionInfo> functionInfo; // 函数信息

	SymbolInfo resolve(const std::string& name, bool& isGlobalVar);

	virtual void visit(ProgramNode& node) override;
	virtual void visit(FunctionDeclareNode& node) override;
	virtual void visit(CodeBlockNode& node) override;
	virtual void visit(IntDeclareNode& node) override;
	virtual void visit(DoubleDeclareNode& node) override;
	virtual void visit(VarNode& node) override;
	virtual void visit(VarAssignNode& node) override;
	virtual void visit(ReturnNode& node) override;
	virtual void visit(FunctionCallStmtNode& node) override;
	virtual void visit(FunctionCallExprNode& node) override;
};