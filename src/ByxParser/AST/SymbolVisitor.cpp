#include "SymbolVisitor.h"
#include "../ByxParser.h"
#include "../FunctionInfo.h"
#include "ToStringVisitor.h"


#include <iostream>

using namespace std;

SymbolVisitor::SymbolVisitor()
{
	inGlobalScope = false;
	globalVarIndex = 0;
	funcIndex = 1;
}

int SymbolVisitor::getGlobalSpace()
{
	return globalVarInfo.size();
}

FunctionTable SymbolVisitor::getFunctionTable()
{
	// 检查是否有main函数
	if (functionInfo.count("main") == 0)
	{
		throw ByxParser::ParseError("Cannot find function main.", 0, 0);
	}

	// 构造函数表
	FunctionTable functionTable;
	for (int i = 0; i < (int)functionInfo.size(); ++i)
	{
		functionTable.add(0, 0);
	}

	// 设置函数局部变量空间
	for (auto i = functionInfo.begin(); i != functionInfo.end(); ++i)
	{
		functionTable.setSpace(i->second.index, i->second.space);
	}

	return functionTable;
}

std::map<std::string, FunctionInfo> SymbolVisitor::getFunctionInfo()
{
	return functionInfo;
}

SymbolInfo SymbolVisitor::resolve(const std::string& name, bool& isGlobalVar)
{
	try
	{
		// 查找局部作用域
		SymbolInfo symbol = scopeStack.resolve(name);
		isGlobalVar = false;
		return symbol;
	}
	catch (...)
	{
		// 查找全局作用域
		if (globalVarInfo.count(name) == 0)
		{
			throw 1;
		}

		isGlobalVar = true;
		return globalVarInfo[name];
	}
}

void SymbolVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		inGlobalScope = true;
		node.stmts[i]->visit(*this);
	}
}

void SymbolVisitor::visit(FunctionDeclareNode& node)
{
	// 检测重复定义函数
	if (functionInfo.count(node.name) > 0)
	{
		throw ByxParser::ParseError(string("Redefined function: ") + node.name + ".", node.row(), node.col());
	}

	int index = 0;
	if (node.name == "main") // 验证main函数参数和返回值
	{
		if (node.retType != DataType::Void)
		{
			throw ByxParser::ParseError("Main function cannot have return value.", node.row(), node.col());
		}

		if (node.paramType.size() != 0)
		{
			throw ByxParser::ParseError("Main function cannot have any parameters.", node.row(), node.col());
		}
	}
	else
	{
		index = funcIndex++;
	}

	inGlobalScope = false;

	// 初始化作用域栈
	scopeStack = ScopeStack();
	scopeStack.pushScope();

	// 处理参数
	for (int i = 0; i < (int)node.paramType.size(); ++i)
	{
		scopeStack.define(node.paramName[i], node.paramType[i]);
	}

	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// 记录函数相关信息
	functionInfo[node.name] = FunctionInfo(index, scopeStack.getSpace(), node.paramType.size(), node.retType);

	// 弹出作用域
	scopeStack.popScope();
}

void SymbolVisitor::visit(CodeBlockNode& node)
{
	scopeStack.pushScope();

	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}

	scopeStack.popScope();
}

void SymbolVisitor::visit(IntDeclareNode& node)
{
	// 全局变量
	if (inGlobalScope)
	{
		// 全局变量重定义
		if (globalVarInfo.count(node.name) > 0)
		{
			throw ByxParser::ParseError(string("Global var '") + node.name + "' is redefined.", node.row(), node.col());
		}

		globalVarInfo[node.name] = SymbolInfo(DataType::Integer, globalVarIndex++, true);
		node.symbol.index = globalVarIndex - 1;
		node.symbol.isGlobal = true;
	}
	// 局部变量
	else
	{
		try
		{
			int index = scopeStack.define(node.name, DataType::Integer);
			node.symbol.index = index;
			node.symbol.isGlobal = false;
		}
		catch (...)
		{
			throw ByxParser::ParseError(string("Redefined symbol: ") + node.name + ".", node.row(), node.col());
		}
	}

	node.expr->visit(*this);
}

void SymbolVisitor::visit(DoubleDeclareNode& node)
{
	// 全局变量
	if (inGlobalScope)
	{
		// 全局变量重定义
		if (globalVarInfo.count(node.name) > 0)
		{
			throw ByxParser::ParseError(string("Global var '") + node.name + "' is redefined.", node.row(), node.col());
		}

		globalVarInfo[node.name] = SymbolInfo(DataType::Double, globalVarIndex++, true);
		node.symbol.index = globalVarIndex - 1;
		node.symbol.isGlobal = true;
	}
	// 局部变量
	else
	{
		try
		{
			int index = scopeStack.define(node.name, DataType::Double);
			node.symbol.index = index;
			node.symbol.isGlobal = false;
		}
		catch (...)
		{
			throw ByxParser::ParseError(string("Redefined symbol: ") + node.name + ".", node.row(), node.col());
		}
	}

	node.expr->visit(*this);
}

void SymbolVisitor::visit(VarNode& node)
{
	try
	{
		SymbolInfo symbol = resolve(node.name, node.symbol.isGlobal);
		node.symbol.index = symbol.index;
		node.type = symbol.type;
	}
	catch (...)
	{
		throw ByxParser::ParseError(string("Undefined symbol: ") + node.name + ".", node.row(), node.col());
	}
}

void SymbolVisitor::visit(VarAssignNode& node)
{
	try
	{
		SymbolInfo symbol = resolve(node.name, node.symbol.isGlobal);
		node.symbol.index = symbol.index;
		node.symbol.type = symbol.type;
	}
	catch (...)
	{
		throw ByxParser::ParseError(string("Undefined symbol: ") + node.name + ".", node.row(), node.col());
	}

	node.expr->visit(*this);
}

void SymbolVisitor::visit(ReturnNode& node)
{
	if (node.hasExpr)
	{
		node.expr->visit(*this);
	}
}

void SymbolVisitor::visit(FunctionCallStmtNode& node)
{
	for (int i = 0; i < (int)node.exprs.size(); ++i)
	{
		node.exprs[i]->visit(*this);
	}
}

void SymbolVisitor::visit(FunctionCallExprNode& node)
{
	for (int i = 0; i < (int)node.exprs.size(); ++i)
	{
		node.exprs[i]->visit(*this);
	}
}

void SymbolVisitor::visit(IfNode& node)
{
	node.cond->visit(*this);

	scopeStack.pushScope();
	node.tBranch->visit(*this);
	scopeStack.popScope();

	scopeStack.pushScope();
	node.fBranch->visit(*this);
	scopeStack.popScope();
}
