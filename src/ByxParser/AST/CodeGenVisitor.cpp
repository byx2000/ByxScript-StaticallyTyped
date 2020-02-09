#include "CodeGenVisitor.h"
#include "../ByxParser.h"

#include <iostream>

using namespace std;

CodeGenVisitor::CodeGenVisitor(ByxParser& parser)
	: inGlobal(inGlobal), parser(parser)
{
	inGlobal = false;
	inLoop = false;
}

CodeSeg CodeGenVisitor::getCode() const
{
	return codeSeg;
}

void CodeGenVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		inGlobal = true;
		node.stmts[i]->visit(*this);
	}
}

void CodeGenVisitor::visit(FunctionDeclareNode& node)
{
	// 获取函数信息
	FunctionInfo info = parser.functionInfo[node.name];

	// 设置函数起始地址
	parser.functionTable.setAddr(info.index, codeSeg.getSize());

	// 为main函数添加全局变量初始化代码
	if (node.name == "main")
	{
		codeSeg.add(parser.globalCode);
	}

	// 生成参数读取指令
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
	inGlobal = false;
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// 添加ret指令
	codeSeg.add(Opcode::ret);
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
	if (inGlobal)
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
	if (inGlobal)
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
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
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
	if (node.hasExpr)
	{
		node.expr->visit(*this);
	}
	codeSeg.add(Opcode::ret);
}

void CodeGenVisitor::visit(FunctionCallStmtNode& node)
{
	// 函数未定义
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// 获取调用函数信息
	FunctionInfo callInfo = parser.functionInfo[node.name];

	// 参数个数不匹配
	if (callInfo.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// 生成计算参数的代码（倒序）
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// 生成调用函数代码
	codeSeg.add(Opcode::call, callInfo.index);
	if (callInfo.retType != DataType::Void)
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

	// 获取调用函数信息
	FunctionInfo callInfo = parser.functionInfo[node.name];

	// 函数无返回值
	if (callInfo.retType == DataType::Void)
	{
		throw ByxParser::ParseError(string("Function '") + node.name + "' has no return type.", node.row(), node.col());
	}

	// 参数个数不匹配
	if (callInfo.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// 生成计算参数的代码（倒序）
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// 生成调用函数代码
	codeSeg.add(Opcode::call, callInfo.index);

	// 根据返回值类型进行类型转换
	if (callInfo.retType == DataType::Integer)
	{
		codeSeg.add(Opcode::toi);
	}
	else if (callInfo.retType == DataType::Double)
	{
		codeSeg.add(Opcode::tod);
	}
}

void CodeGenVisitor::visit(IfNode& node)
{
	// 生成条件表达式代码
	node.cond->visit(*this);

	// 若条件为假，则跳转到false分支（跳转目标待定）
	int index1 = codeSeg.add(Opcode::je, 0);

	// 生成true分支代码
	node.tBranch->visit(*this);

	// 执行完true分支后，跳转到if语句结束（跳转目标待定）
	int index2 = codeSeg.add(Opcode::jmp, 0);

	// 设置跳转目标1
	codeSeg.setIntParam(index1, codeSeg.getSize());

	//生成false分支代码
	node.fBranch->visit(*this);

	// 设置跳转目标2
	codeSeg.setIntParam(index2, codeSeg.getSize());
}

void CodeGenVisitor::visit(BinaryOpNode& node)
{
	if (node.opType != BinaryOpNode::And && node.opType != BinaryOpNode::Or)
	{
		node.lhs->visit(*this);
		node.rhs->visit(*this);
	}

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
	case BinaryOpNode::Rem:
		codeSeg.add(Opcode::rem);
		break;
	case BinaryOpNode::Equ:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ie);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::de);
		}
		break;
	case BinaryOpNode::NotEqu:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ine);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dne);
		}
		break;
	case BinaryOpNode::Larger:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ig);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dg);
		}
		break;
	case BinaryOpNode::Less:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::il);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dl);
		}
		break;
	case BinaryOpNode::LargerEqu:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ige);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dge);
		}
		break;
	case BinaryOpNode::LessEqu:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ile);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dle);
		}
		break;
	case BinaryOpNode::And:
	{
		node.lhs->visit(*this);
		int index1 = codeSeg.add(Opcode::je, 0);
		codeSeg.add(Opcode::iconst, 1);
		node.rhs->visit(*this);
		codeSeg.add(Opcode::land);
		int index2 = codeSeg.add(Opcode::jmp, 0);
		codeSeg.setIntParam(index1, codeSeg.getSize());
		codeSeg.add(Opcode::iconst, 0);
		codeSeg.setIntParam(index2, codeSeg.getSize());
		break;
	}
	case BinaryOpNode::Or:
	{
		node.lhs->visit(*this);
		int index1 = codeSeg.add(Opcode::jne, 0);
		codeSeg.add(Opcode::iconst, 0);
		node.rhs->visit(*this);
		codeSeg.add(Opcode::lor);
		int index2 = codeSeg.add(Opcode::jmp, 0);
		codeSeg.setIntParam(index1, codeSeg.getSize());
		codeSeg.add(Opcode::iconst, 1);
		codeSeg.setIntParam(index2, codeSeg.getSize());
		break;
	}
	default:
		throw ByxParser::ParseError("Bad operator.", node.row(), node.col());
		break;
	}
}

void CodeGenVisitor::visit(WhileNode& node)
{
	vector<int> oldBreakStmtIndex = breakStmtIndex;
	breakStmtIndex.clear();
	vector<int> oldContinueBreakStmtIndex = continueStmtIndex;
	continueStmtIndex.clear();
	bool oldInLoop = inLoop;

	inLoop = true;

	// 保存条件判断起始地址
	int addr = codeSeg.getSize();

	// 生成条件表达式代码
	node.cond->visit(*this);

	// 若条件为假，则跳转到循环结束（目标待定）
	int index = codeSeg.add(Opcode::je, 0);

	// 生成循环体代码
	node.body->visit(*this);

	// 循环体执行完后，跳转回条件判断
	codeSeg.add(Opcode::jmp, addr);

	// 设置跳转目标
	codeSeg.setIntParam(index, codeSeg.getSize());

	// 若有break语句，则设置break语句跳转目标
	for (int i = 0; i < (int)breakStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(breakStmtIndex[i], codeSeg.getSize());
	}
	breakStmtIndex = oldBreakStmtIndex;

	// 若有continue语句，则设置continue语句跳转目标
	for (int i = 0; i < (int)continueStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(continueStmtIndex[i], addr);
	}
	continueStmtIndex = oldContinueBreakStmtIndex;

	inLoop = oldInLoop;
}

void CodeGenVisitor::visit(BreakNode& node)
{
	// 不在循环内
	if (!inLoop)
	{
		throw ByxParser::ParseError("Break statement must be in a loop.", node.row(), node.col());
	}

	int index = codeSeg.add(Opcode::jmp, 0);
	breakStmtIndex.push_back(index);
}

void CodeGenVisitor::visit(ContinueNode& node)
{
	// 不在循环内
	if (!inLoop)
	{
		throw ByxParser::ParseError("Break statement must be in a loop.", node.row(), node.col());
	}

	int index = codeSeg.add(Opcode::jmp, 0);
	continueStmtIndex.push_back(index);
}

void CodeGenVisitor::visit(UnaryOpNode& node)
{
	node.expr->visit(*this);

	switch (node.opType)
	{
	case UnaryOpNode::Pos:
		break;
	case UnaryOpNode::Neg:
		if (node.dataType == DataType::Integer)
		{
			codeSeg.add(Opcode::ineg);
		}
		else if (node.dataType == DataType::Double)
		{
			codeSeg.add(Opcode::dneg);
		}
		break;
	case UnaryOpNode::Not:
		codeSeg.add(Opcode::lnot);
		break;
	default:
		throw ByxParser::ParseError("Bad operator.", node.row(), node.col());
		break;
	}
}

void CodeGenVisitor::visit(ForNode& node)
{
	vector<int> oldBreakStmtIndex = breakStmtIndex;
	breakStmtIndex.clear();
	vector<int> oldContinueBreakStmtIndex = continueStmtIndex;
	continueStmtIndex.clear();
	bool oldInLoop = inLoop;
	inLoop = true;

	// 生成初始化代码
	node.init->visit(*this);

	// 记录条件判断起始地址
	int addrCond = codeSeg.getSize();

	// 生成条件判断代码
	node.cond->visit(*this);

	// 若条件为假，则跳转到循环结束
	int index = codeSeg.add(Opcode::je, 0);

	// 生成循环体代码
	node.body->visit(*this);

	// 记录更新代码起始地址
	int addrUpdate = codeSeg.getSize();

	// 生成更新代码
	node.update->visit(*this);

	// 跳转到条件判断
	codeSeg.add(Opcode::jmp, addrCond);

	// 回填跳转地址
	codeSeg.setIntParam(index, codeSeg.getSize());

	// 若有break语句，则设置break语句跳转目标
	for (int i = 0; i < (int)breakStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(breakStmtIndex[i], codeSeg.getSize());
	}
	breakStmtIndex = oldBreakStmtIndex;

	// 若有continue语句，则设置continue语句跳转目标
	for (int i = 0; i < (int)continueStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(continueStmtIndex[i], addrUpdate);
	}
	continueStmtIndex = oldContinueBreakStmtIndex;

	inLoop = oldInLoop;
}
