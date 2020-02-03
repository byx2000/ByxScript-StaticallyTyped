#include "FuncCombVisitor.h"
#include "FuncCodeGenVisitor.h"
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

	// ���ɺ��������
	FuncCodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	codeSeg.add(seg);
}
