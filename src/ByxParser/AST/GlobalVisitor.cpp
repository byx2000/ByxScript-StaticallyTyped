#include "GlobalVisitor.h"
#include "../ByxParser.h"
#include "FuncCodeGenVisitor.h"

using namespace std;

GlobalVisitor::GlobalVisitor(ByxParser& parser)
	: parser(parser)
{
	funcIndex = 1;
	varIndex = 0;
}

CodeSeg GlobalVisitor::getInitCode()
{
	return initCode;
}

void GlobalVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void GlobalVisitor::visit(IntDeclareNode& node)
{
	/*CodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCodeSeg();
	initCode.add(seg);*/

	FuncCodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	initCode.add(seg);
}

void GlobalVisitor::visit(DoubleDeclareNode& node)
{	
	/*CodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCodeSeg();
	initCode.add(seg);*/

	FuncCodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	initCode.add(seg);
}
