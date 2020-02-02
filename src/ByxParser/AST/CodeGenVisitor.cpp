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

std::vector<Instruction> CodeGenVisitor::getCode()
{
	return insts;
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
	int instCount = insts.size();


	// ��ȡ��������
	FunctionInfo info = parser.functionInfo[node.name];
	int index = info.index;

	// ���ú�����ʼ��ַ
	parser.functionTable.setAddr(index, insts.size());

	// Ϊmain�������ȫ�ֱ�����ʼ������
	if (node.name == "main")
	{
		for (int i = 0; i < (int)parser.initCode.size(); ++i)
		{
			insts.push_back(parser.initCode[i]);
		}
	}

	// �������
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

	// ���ɺ��������
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// ���retָ��ô����ܻ������ظ���retָ�
	insts.push_back(Instruction(Opcode::ret));

	curFunctionName = "";
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
	// ����ȫ�ֱ�������
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
	// ����ȫ�ֱ�������
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
	// ���ɴ���
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
	// ���ɸ�ֵ���ʽ����
	node.expr->visit(*this);

	// ȫ�ֱ���
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
	// �ֲ�����
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
	// return��䲻�ں�����
	if (curFunctionName == "")
	{
		throw ByxParser::ParseError("Return statement must be in a function.", node.row(), node.col());
	}

	// ����������
	if (parser.functionInfo.count(curFunctionName) == 0)
	{
		throw ByxParser::ParseError(string("Function '") + curFunctionName + "' does not exist.", node.row(), node.col());
	}


	// ��ȡ��������ֵ��Ϣ
	FunctionInfo info = parser.functionInfo[curFunctionName];
	DataType retType = info.retType;

	// ���ɴ���
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
	// ����δ����
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// ��ȡ������Ϣ
	FunctionInfo info = parser.functionInfo[node.name];
	
	// ����������ƥ��
	if (info.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// ���ɼ�������Ĵ��루����
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// ���ɵ��ú�������
	insts.push_back(Instruction(Opcode::call, info.index));
	if (info.retType != DataType::Void)
	{
		insts.push_back(Instruction(Opcode::pop));
	}
}

void CodeGenVisitor::visit(FunctionCallExprNode& node)
{
	// ����δ����
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// ��ȡ������Ϣ
	FunctionInfo info = parser.functionInfo[node.name];

	// �����޷���ֵ
	if (info.retType == DataType::Void)
	{
		throw ByxParser::ParseError(string("Function '") + node.name + "' has no return type.", node.row(), node.col());
	}

	// ����������ƥ��
	if (info.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// ���ɼ�������Ĵ��루����
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// ���ɵ��ú�������
	insts.push_back(Instruction(Opcode::call, info.index));

	// ���ݷ���ֵ���ͽ�������ת��
	if (info.retType == DataType::Integer)
	{
		insts.push_back(Instruction(Opcode::toi));
	}
	else if (info.retType == DataType::Double)
	{
		insts.push_back(Instruction(Opcode::tod));
	}
}

void CodeGenVisitor::visit(IfNode& node)
{
	CodeGenVisitor v1(parser, curFunctionName);
	node.cond->visit(v1);
	vector<Instruction> condCode = v1.getCode();
	//printCode(condCode);

	CodeGenVisitor v2(parser, curFunctionName);
	node.tBranch->visit(v2);
	vector<Instruction> tBranchCode = v2.getCode();
	//printCode(tBranchCode);

	CodeGenVisitor v3(parser, curFunctionName);
	node.fBranch->visit(v3);
	vector<Instruction> fBranchCode = v3.getCode();
	//printCode(fBranchCode);

	for (int i = 0; i < (int)condCode.size(); ++i)
	{
		insts.push_back(condCode[i]);
	}
	insts.push_back(Instruction(Opcode::je, (int)insts.size() + (int)tBranchCode.size() + 2));
	for (int i = 0; i < (int)tBranchCode.size(); ++i)
	{
		insts.push_back(tBranchCode[i]);
	}
	insts.push_back(Instruction(Opcode::jmp, (int)insts.size() + (int)fBranchCode.size() + 1));
	for (int i = 0; i < (int)fBranchCode.size(); ++i)
	{
		insts.push_back(fBranchCode[i]);
	}
}
