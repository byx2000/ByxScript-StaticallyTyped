#include "FuncCombVisitor.h"
#include "CodeGenVisitor.h"
#include "../FunctionInfo.h"
#include "../ByxParser.h"

#include <iostream>

using namespace std;

FuncCombVisitor::FuncCombVisitor(ByxParser& parser)
	: parser(parser)
{

}

CodeSeg FuncCombVisitor::getCode() const
{
	return codeSeg;
}

void FuncCombVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void FuncCombVisitor::visit(FunctionDeclareNode& node)
{
	// ��ȡ������Ϣ
	FunctionInfo info = parser.functionInfo[node.name];

	// ���ú�����ʼ��ַ
	parser.functionTable.setAddr(info.index, codeSeg.getSize());

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
	CodeGenVisitor visitor(parser, node.name);
	//node.visit(visitor);
	
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(visitor);
	}

	CodeSeg seg = visitor.getCode();
	codeSeg.add(seg);

	// ���retָ��ô����ܻ������ظ���retָ�
	codeSeg.add(Opcode::ret);
}
