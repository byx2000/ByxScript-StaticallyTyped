#include "CodeGenVisitor.h"
#include "../ByxParser.h"
#include "../FunctionInfo.h"

#include <iostream>

using namespace std;

CodeGenVisitor::CodeGenVisitor(ByxParser& parser)
	: parser(parser)
{
	inGlobleScope = false;
	curFunctionName = "";
}

std::vector<Instruction> CodeGenVisitor::getCode()
{
	return insts;
}

void CodeGenVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		inGlobleScope = true;
		node.stmts[i]->visit(*this);
	}
}

void CodeGenVisitor::visit(FunctionDeclareNode& node)
{
	inGlobleScope = false;
	curFunctionName = node.name;
	int instCount = insts.size();


	// 获取函数索引
	FunctionInfo info = parser.functionInfo[node.name];
	int index = info.index;

	// 设置函数起始地址
	parser.functionTable.setAddr(index, insts.size());

	// 为main函数添加全局变量初始化代码
	if (node.name == "main")
	{
		for (int i = 0; i < (int)parser.initCode.size(); ++i)
		{
			insts.push_back(parser.initCode[i]);
		}
	}

	// 处理参数
	int cnt = 0;
	for (int i = 0; i < (int)node.paramName.size(); ++i)
	{
		if (node.paramType[i] == DataType::Integer)
		{
			insts.push_back(Instruction(Opcode::istore, cnt));
		}
		else if (node.paramType[i] == DataType::Double)
		{
			insts.push_back(Instruction(Opcode::dstore, cnt));
		}
		cnt++;
	}

	// 生成函数体代码
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// 添加ret指令（该处可能会生成重复的ret指令）
	insts.push_back(Instruction(Opcode::ret));
}

void CodeGenVisitor::visit(IntegerNode& node)
{
	insts.push_back(Instruction(Opcode::iconst, node.val));
}

void CodeGenVisitor::visit(DoubleNode& node)
{
	insts.push_back(Instruction(Opcode::dconst, node.val));
}

void CodeGenVisitor::visit(IntDeclareNode& node)
{
	// 跳过全局变量声明
	if (inGlobleScope)
	{
		return;
	}

	node.expr->visit(*this);
	if (node.symbol.isGlobal)
	{
		insts.push_back(Instruction(Opcode::igstore, node.symbol.index));
	}
	else
	{
		insts.push_back(Instruction(Opcode::istore, node.symbol.index));
	}
}

void CodeGenVisitor::visit(DoubleDeclareNode& node)
{
	// 跳过全局变量声明
	if (inGlobleScope)
	{
		return;
	}

	node.expr->visit(*this);
	if (node.symbol.isGlobal)
	{
		insts.push_back(Instruction(Opcode::dgstore, node.symbol.index));
	}
	else
	{
		insts.push_back(Instruction(Opcode::dstore, node.symbol.index));
	}
}

void CodeGenVisitor::visit(CodeBlockNode& node)
{
	// 生成代码
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void CodeGenVisitor::visit(VarNode& node)
{
	if (node.type == DataType::Integer)
	{
		if (node.symbol.isGlobal)
		{
			insts.push_back(Instruction(Opcode::igload, node.symbol.index));
		}
		else
		{
			insts.push_back(Instruction(Opcode::iload, node.symbol.index));
		}
	}
	else if (node.type == DataType::Double)
	{
		if (node.symbol.isGlobal)
		{
			insts.push_back(Instruction(Opcode::dgload, node.symbol.index));
		}
		else
		{
			insts.push_back(Instruction(Opcode::dload, node.symbol.index));
		}
	}
}

void CodeGenVisitor::visit(VarAssignNode& node)
{
	// 生成赋值表达式代码
	node.expr->visit(*this);

	// 全局变量
	if (node.symbol.isGlobal)
	{
		if (node.symbol.type == DataType::Integer)
		{
			insts.push_back(Instruction(Opcode::igstore, node.symbol.index));
		}
		else if (node.symbol.type == DataType::Double)
		{
			insts.push_back(Instruction(Opcode::dgstore, node.symbol.index));
		}
	}
	// 局部变量
	else
	{
		if (node.symbol.type == DataType::Integer)
		{
			insts.push_back(Instruction(Opcode::istore, node.symbol.index));
		}
		else if (node.symbol.type == DataType::Double)
		{
			insts.push_back(Instruction(Opcode::dstore, node.symbol.index));
		}
	}
}

void CodeGenVisitor::visit(ReturnNode& node)
{
	// return语句不在函数内
	if (curFunctionName == "")
	{
		throw ByxParser::ParseError("Return statement must be in a function.", node.row(), node.col());
	}

	// 函数不存在
	if (parser.functionInfo.count(curFunctionName) == 0)
	{
		throw ByxParser::ParseError(string("Function '") + curFunctionName + "' does not exist.", node.row(), node.col());
	}


	// 获取函数返回值信息
	FunctionInfo info = parser.functionInfo[curFunctionName];
	DataType retType = info.retType;

	// 生成代码
	if (retType == DataType::Void)
	{
		if (node.hasExpr)
		{
			throw ByxParser::ParseError(string("Function '") + curFunctionName + "' cannot return value.", node.row(), node.col());
		}
		else
		{
			insts.push_back(Instruction(Opcode::ret));
		}
	}
	else
	{
		if (!node.hasExpr)
		{
			throw ByxParser::ParseError(string("Function '") + curFunctionName + "'must return a value.", node.row(), node.col());
		}
		else
		{
			node.expr->visit(*this);
			insts.push_back(Instruction(Opcode::ret));
		}
	}
}

void CodeGenVisitor::visit(FunctionCallStmtNode& node)
{
	// 函数未定义
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// 获取函数信息
	FunctionInfo info = parser.functionInfo[node.name];
	
	// 参数个数不匹配
	if (info.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// 生成计算参数的代码（倒序）
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// 生成调用函数代码
	insts.push_back(Instruction(Opcode::call, info.index));
	if (info.retType != DataType::Void)
	{
		insts.push_back(Instruction(Opcode::pop));
	}
}

void CodeGenVisitor::visit(FunctionCallExprNode& node)
{
	// 函数未定义
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// 获取函数信息
	FunctionInfo info = parser.functionInfo[node.name];

	// 函数无返回值
	if (info.retType == DataType::Void)
	{
		throw ByxParser::ParseError(string("Function '") + node.name + "' has no return type.", node.row(), node.col());
	}

	// 参数个数不匹配
	if (info.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// 生成计算参数的代码（倒序）
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// 生成调用函数代码
	insts.push_back(Instruction(Opcode::call, info.index));

	// 根据返回值类型进行类型转换
	if (info.retType == DataType::Integer)
	{
		insts.push_back(Instruction(Opcode::toi));
	}
	else if (info.retType == DataType::Double)
	{
		insts.push_back(Instruction(Opcode::tod));
	}
}
