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
	bool inGlobalScope; // ����Ƿ���ȫ��������
	int globalVarIndex; // ���ڼ���ȫ�ֱ�������
	int funcIndex; // ���ڼ��㺯������
	ScopeStack scopeStack; // �ֲ�������ջ
	std::map<std::string, SymbolInfo> globalVarInfo; // ȫ�ֱ�����Ϣ
	std::map<std::string, FunctionInfo> functionInfo; // ������Ϣ

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