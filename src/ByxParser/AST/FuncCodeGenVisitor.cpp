#include "FuncCodeGenVisitor.h"
#include "../ByxParser.h"

#include <iostream>

using namespace std;

FuncCodeGenVisitor::FuncCodeGenVisitor(ByxParser& parser, const FunctionInfo& info)
	: parser(parser), info(info)
{
	inLoop = false;
}

CodeSeg FuncCodeGenVisitor::getCode() const
{
	return codeSeg;
}

void FuncCodeGenVisitor::visit(FunctionDeclareNode& node)
{
	// ��ȡ������Ϣ
	info = parser.functionInfo[node.name];
	funcName = node.name;

	// Ϊmain�������ȫ�ֱ�����ʼ������
	if (node.name == "main")
	{
		codeSeg.add(parser.initCode);
	}

	// ���ɲ�����ȡָ��
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

	// ���ɺ��������
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
	}

	// ���retָ��ô����ܻ������ظ���retָ�
	codeSeg.add(Opcode::ret);
}

void FuncCodeGenVisitor::visit(IntegerNode& node)
{
	codeSeg.add(Opcode::iconst, node.val);
}

void FuncCodeGenVisitor::visit(DoubleNode& node)
{
	codeSeg.add(Opcode::dconst, node.val);
}

void FuncCodeGenVisitor::visit(IntDeclareNode& node)
{
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

void FuncCodeGenVisitor::visit(DoubleDeclareNode& node)
{
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

void FuncCodeGenVisitor::visit(CodeBlockNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void FuncCodeGenVisitor::visit(VarNode& node)
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

void FuncCodeGenVisitor::visit(VarAssignNode& node)
{
	// ���ɸ�ֵ���ʽ����
	node.expr->visit(*this);

	// ȫ�ֱ���
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
	// �ֲ�����
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

void FuncCodeGenVisitor::visit(ReturnNode& node)
{
	// ��ȡ��������ֵ��Ϣ
	DataType retType = info.retType;

	// ���ɴ���
	if (retType == DataType::Void)
	{
		if (node.hasExpr) // ����ֵ���Ͳ�ƥ��
		{
			throw ByxParser::ParseError(string("Function '") + funcName + "' cannot return value.", node.row(), node.col());
		}
		else
		{
			codeSeg.add(Opcode::ret);
		}
	}
	else
	{
		if (!node.hasExpr) // ����ֵ���Ͳ�ƥ��
		{
			throw ByxParser::ParseError(string("Function '") + funcName + "'must return a value.", node.row(), node.col());
		}
		else
		{
			node.expr->visit(*this);
			codeSeg.add(Opcode::ret);
		}
	}
}

void FuncCodeGenVisitor::visit(FunctionCallStmtNode& node)
{
	// ����δ����
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// ��ȡ���ú�����Ϣ
	FunctionInfo callInfo = parser.functionInfo[node.name];

	// ����������ƥ��
	if (callInfo.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// ���ɼ�������Ĵ��루����
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// ���ɵ��ú�������
	codeSeg.add(Opcode::call, callInfo.index);
	if (callInfo.retType != DataType::Void)
	{
		codeSeg.add(Opcode::pop);
	}
}

void FuncCodeGenVisitor::visit(FunctionCallExprNode& node)
{
	// ����δ����
	if (parser.functionInfo.count(node.name) == 0)
	{
		throw ByxParser::ParseError(string("Undefined function: ") + node.name + ".", node.row(), node.col());
	}

	// ��ȡ���ú�����Ϣ
	FunctionInfo callInfo = parser.functionInfo[node.name];

	// �����޷���ֵ
	if (callInfo.retType == DataType::Void)
	{
		throw ByxParser::ParseError(string("Function '") + node.name + "' has no return type.", node.row(), node.col());
	}

	// ����������ƥ��
	if (callInfo.paramCount != node.exprs.size())
	{
		throw ByxParser::ParseError(string("The count of parameters incorrect when call function ") + node.name + ".", node.row(), node.col());
	}

	// ���ɼ�������Ĵ��루����
	for (int i = (int)node.exprs.size() - 1; i >= 0; --i)
	{
		node.exprs[i]->visit(*this);
	}

	// ���ɵ��ú�������
	codeSeg.add(Opcode::call, callInfo.index);

	// ���ݷ���ֵ���ͽ�������ת��
	if (callInfo.retType == DataType::Integer)
	{
		codeSeg.add(Opcode::toi);
	}
	else if (callInfo.retType == DataType::Double)
	{
		codeSeg.add(Opcode::tod);
	}
}

void FuncCodeGenVisitor::visit(IfNode& node)
{
	/*FuncCodeGenVisitor v1(parser, info);
	node.cond->visit(v1);
	CodeSeg condCode = v1.getCode();

	FuncCodeGenVisitor v2(parser, info);
	node.tBranch->visit(v2);
	CodeSeg tBranchCode = v2.getCode();

	FuncCodeGenVisitor v3(parser, info);
	node.fBranch->visit(v3);
	CodeSeg fBranchCode = v3.getCode();

	codeSeg.add(condCode);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}
	codeSeg.add(Opcode::je, codeSeg.getSize() + tBranchCode.getSize() + 2);
	codeSeg.add(tBranchCode);
	codeSeg.add(Opcode::jmp, codeSeg.getSize() + fBranchCode.getSize() + 1);
	codeSeg.add(fBranchCode);*/

	/*ifNestedDepth++;
	string label1 = string("false_start") + to_string(ifNestedDepth);
	string label2 = string("if_end") + to_string(ifNestedDepth);

	// �����������ʽ����
	node.cond->visit(*this);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}

	// ������Ϊ�٣�����ת��false��֧
	codeSeg.addJumpLabel(Opcode::je, label1);

	// ����true��֧����
	node.tBranch->visit(*this);

	// ִ����true��֧����ת��if������
	codeSeg.addJumpLabel(Opcode::jmp, label2);

	// ����false��֧��ʼ��ַ
	codeSeg.setJumpLabel(label1, codeSeg.getSize());

	//����false��֧����
	node.fBranch->visit(*this);

	// ����if��������ַ
	codeSeg.setJumpLabel(label2, codeSeg.getSize());

	ifNestedDepth--;*/

	// �����������ʽ����
	node.cond->visit(*this);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}

	// ������Ϊ�٣�����ת��false��֧����תĿ�������
	int index1 = codeSeg.add(Opcode::je, 0);

	// ����true��֧����
	node.tBranch->visit(*this);

	// ִ����true��֧����ת��if����������תĿ�������
	int index2 = codeSeg.add(Opcode::jmp, 0);

	// ������תĿ��1
	codeSeg.setIntParam(index1, codeSeg.getSize());

	//����false��֧����
	node.fBranch->visit(*this);

	// ������תĿ��2
	codeSeg.setIntParam(index2, codeSeg.getSize());
}

void FuncCodeGenVisitor::visit(BinaryOpNode& node)
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
		/*node.lhs->visit(*this);
		int index = codeSeg.add(Opcode::je, 0);
		node.rhs->visit(*this);
		codeSeg.add(Opcode::iand);*/
		break;
	}
	case BinaryOpNode::Or:
		break;
	default:
		throw ByxParser::ParseError("Bad operator.", node.row(), node.col());
		break;
	}
}

void FuncCodeGenVisitor::visit(WhileNode& node)
{
	/*FuncCodeGenVisitor v1(parser, info);
	node.cond->visit(v1);
	CodeSeg condCode = v1.getCode();

	FuncCodeGenVisitor v2(parser, info);
	node.body->visit(v2);
	CodeSeg bodyCode = v2.getCode();

	int addr = codeSeg.getSize();

	codeSeg.add(condCode);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}
	codeSeg.add(Opcode::je, codeSeg.getSize() + bodyCode.getSize() + 2);
	codeSeg.add(bodyCode);
	codeSeg.add(Opcode::jmp, addr);*/

	/*whileNestedDepth++;

	string label = string("while_") + to_string(whileNestedDepth);

	// ���������ж���ʼ��ַ
	int addr = codeSeg.getSize();

	// �����������ʽ����
	node.cond->visit(*this);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}

	// ������Ϊ�٣�����ת��ѭ������
	codeSeg.addJumpLabel(Opcode::je, label);

	// ����ѭ�������
	node.body->visit(*this);

	// ѭ����ִ�������ת�������ж�
	codeSeg.add(Opcode::jmp, addr);

	// ����ѭ��������ַ
	codeSeg.setJumpLabel(label, codeSeg.getSize());

	// ����break�����תĿ��
	codeSeg.setJumpLabel(string("break_") + to_string(whileNestedDepth), codeSeg.getSize());

	whileNestedDepth--;*/

	vector<int> oldBreakStmtIndex = breakStmtIndex;
	breakStmtIndex.clear();
	vector<int> oldContinueBreakStmtIndex = continueStmtIndex;
	continueStmtIndex.clear();
	bool oldInLoop = inLoop;

	inLoop = true;

	// ���������ж���ʼ��ַ
	int addr = codeSeg.getSize();

	// �����������ʽ����
	node.cond->visit(*this);
	if (node.cond->dataType == DataType::Double) // ���⴦������
	{
		codeSeg.add(Opcode::dconst, 0.0);
		codeSeg.add(Opcode::dne);
	}

	// ������Ϊ�٣�����ת��ѭ��������Ŀ�������
	int index = codeSeg.add(Opcode::je, 0);

	// ����ѭ�������
	node.body->visit(*this);

	// ѭ����ִ�������ת�������ж�
	codeSeg.add(Opcode::jmp, addr);

	// ������תĿ��
	codeSeg.setIntParam(index, codeSeg.getSize());

	// ����break��䣬������break�����תĿ��
	for (int i = 0; i < (int)breakStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(breakStmtIndex[i], codeSeg.getSize());
	}
	breakStmtIndex = oldBreakStmtIndex;

	// ����continue��䣬������continue�����תĿ��
	for (int i = 0; i < (int)continueStmtIndex.size(); ++i)
	{
		codeSeg.setIntParam(continueStmtIndex[i], addr);
	}
	continueStmtIndex = oldContinueBreakStmtIndex;

	inLoop = oldInLoop;
}

void FuncCodeGenVisitor::visit(BreakNode& node)
{
	// ����ѭ����
	if (!inLoop)
	{
		throw ByxParser::ParseError("Break statement must be in a loop.", node.row(), node.col());
	}

	int index = codeSeg.add(Opcode::jmp, 0);
	breakStmtIndex.push_back(index);
}

void FuncCodeGenVisitor::visit(ContinueNode& node)
{
	// ����ѭ����
	if (!inLoop)
	{
		throw ByxParser::ParseError("Break statement must be in a loop.", node.row(), node.col());
	}

	int index = codeSeg.add(Opcode::jmp, 0);
	continueStmtIndex.push_back(index);
}
