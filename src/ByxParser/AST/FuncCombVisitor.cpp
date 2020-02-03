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
	// 获取函数信息
	FunctionInfo info = parser.functionInfo[node.name];

	// 设置函数起始地址
	parser.functionTable.setAddr(info.index, codeSeg.getSize());

	// 生成函数体代码
	FuncCodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	codeSeg.add(seg);
}
