#include "CodeGenVisitor.h"
#include "../ByxParser.h"
#include "../FunctionInfo.h"

#include <iostream>

using namespace std;

CodeGenVisitor::CodeGenVisitor(ByxParser& parser, const std::string& curFunctionName)
	: parser(parser), curFunctionName(curFunctionName)
{
	inGlobleScope = false;
}

CodeSeg CodeGenVisitor::getCodeSeg()
{
	return codeSeg;
}

void CodeGenVisitor::printCode(const std::vector<Instruction>& code)
{
	for (int i = 0; i < (int)code.size(); ++i)
	{
		cout << code[i].toString() << endl;
	}
	cout << endl;
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
	int instCount = codeSeg.getSize();


	// 获取函数索引
	FunctionInfo info = parser.functionInfo[node.name];
	int index = info.index;

	// 设置函数起始地址
	parser.functionTable.setAddr(index, instCount);

	// 为main函数添加全局变量初始化代码
	if (node.name == "main")
	{
		codeSeg.add(parser.initCode);
	}

	// 处理参数
	int cnt = 0;
	for (int i = 0; i < (int)node.paramName.size(); ++i)
	{
		if (node.paramType[i] == DataType::Integer)
		{
			codeSeg.add(Opcode::istore, cnt);
		}
		else if (node.paramType[i] == DataType::Double)
		{
			codeSeg.add(Opcode::dstore, cnt);
		}
		cnt++;
	}

	// 生成函数体代码
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// 添加ret指令（该处可能会生成重复的ret指令）
	codeSeg.add(Opcode::ret);

	curFunctionName = "";
}

void CodeGenVisitor::visit(IntegerNode& node)
{
	codeSeg.add(Opcode::iconst, node.val);
}

void CodeGenVisitor::visit(DoubleNode& node)
{
	codeSeg.add(Opcode::dconst, node.val);
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
		codeSeg.add(Opcode::igstore, node.symbol.index);
	}
	else
	{
		codeSeg.add(Opcode::istore, node.symbol.index);
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
		codeSeg.add(Opcode::dgstore, node.symbol.index);
	}
	else
	{
		codeSeg.add(Opcode::dstore, node.symbol.index);
	}
}

void CodeGenVisitor::visit(CodeBlockNode& node)
{
	// 生成代码
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
		/*CodeGenVisitor v(parser, curFunctionName);
		node.stmts[i]->visit(v);
		CodeSeg seg = v.getCodeSeg();
		codeSeg.add(seg);*/
	}
}

void CodeGenVisitor::visit(VarNode& node)
{
	if (node.dataType == DataType::Integer)
	{
		if (node.symbol.isGlobal)
		{
			codeSeg.add(Opcode::igload, node.symbol.index);
		}
		else
		{
			codeSeg.add(Opcode::iload, node.symbol.index);
		}
	}
	else if (node.dataType == DataType::Double)
	{
		if (node.symbol.isGlobal)
		{
			codeSeg.add(Opcode::dgload, node.symbol.index);
		}
		else
		{
			codeSeg.add(Opcode::dload, node.symbol.index);
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
			codeSeg.add(Opcode::igstore, node.symbol.index);
		}
		else if (node.symbol.type == DataType::Double)
		{
			codeSeg.add(Opcode::dgstore, node.symbol.index);
		}
	}
	// 局部变量
	else
	{
		if (node.symbol.type == DataType::Integer)
		{
			codeSeg.add(Opcode::istore, node.symbol.index);
		}
		else if (node.symbol.type == DataType::Double)
		{
			codeSeg.add(Opcode::dstore, node.symbol.index);
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
			codeSeg.add(Opcode::ret);
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
			codeSeg.add(Opcode::ret);
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
	codeSeg.add(Opcode::call, info.index);
	if (info.retType != DataType::Void)
	{
		codeSeg.add(Opcode::pop);
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
	codeSeg.add(Opcode::call, info.index);

	// 根据返回值类型进行类型转换
	if (info.retType == DataType::Integer)
	{
		codeSeg.add(Opcode::toi);
	}
	else if (info.retType == DataType::Double)
	{
		codeSeg.add(Opcode::tod);
	}
}

void CodeGenVisitor::visit(IfNode& node)
{
	CodeGenVisitor v1(parser, curFunctionName);
	node.cond->visit(v1);
	CodeSeg condCode = v1.getCodeSeg();

	CodeGenVisitor v2(parser, curFunctionName);
	node.tBranch->visit(v2);
	CodeSeg tBranchCode = v2.getCodeSeg();

	CodeGenVisitor v3(parser, curFunctionName);
	node.fBranch->visit(v3);
	CodeSeg fBranchCode = v3.getCodeSeg();

	codeSeg.add(condCode);
	if (node.cond->dataType == DataType::Double) // 特殊处理浮点数
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}
	codeSeg.add(Opcode::je, codeSeg.getSize() + tBranchCode.getSize() + 2);
	codeSeg.add(tBranchCode);
	codeSeg.add(Opcode::jmp, codeSeg.getSize() + fBranchCode.getSize() + 1);
	codeSeg.add(fBranchCode);
}

void CodeGenVisitor::visit(BinaryOpNode& node)
{
	node.lhs->visit(*this);
	node.rhs->visit(*this);

	switch (node.opType)
	{
	case BinaryOpNode::Add:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::iadd);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dadd);
		}
		break;
	case BinaryOpNode::Sub:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::isub);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dsub);
		}
		break;
	case BinaryOpNode::Mul:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::imul);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dmul);
		}
		break;
	case BinaryOpNode::Div:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::idiv);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::ddiv);
		}
		break;
	default:
		throw ByxParser::ParseError("Bad operator.", node.row(), node.col());
		break;
	}
}

void CodeGenVisitor::visit(WhileNode& node)
{
	CodeGenVisitor v1(parser, curFunctionName);
	node.cond->visit(v1);
	CodeSeg condCode = v1.getCodeSeg();

	CodeGenVisitor v2(parser, curFunctionName);
	node.body->visit(v2);
	CodeSeg bodyCode = v2.getCodeSeg();

	/*cout << "cond:" << endl;
	cout << condCode.toString() << endl;
	cout << "body:" << endl;
	cout << bodyCode.toString() << endl;*/

	int addr = codeSeg.getSize();

	codeSeg.add(condCode);
	if (node.cond->dataType == DataType::Double) // 特殊处理浮点数
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}
	codeSeg.add(Opcode::je, codeSeg.getSize() + bodyCode.getSize() + 2);
	codeSeg.add(bodyCode);
	codeSeg.add(Opcode::jmp, addr);
}
