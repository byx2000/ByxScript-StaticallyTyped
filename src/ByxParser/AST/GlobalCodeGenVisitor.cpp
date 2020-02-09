#include "GlobalCodeGenVisitor.h"
#include "../ByxParser.h"
#include "CodeGenVisitor.h"

using namespace std;

GlobalCodeGenVisitor::GlobalCodeGenVisitor(ByxParser& parser)
	: parser(parser)
{
	funcIndex = 1;
	varIndex = 0;
}

CodeSeg GlobalCodeGenVisitor::getGlobalCode()
{
	return globalCode;
}

void GlobalCodeGenVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void GlobalCodeGenVisitor::visit(IntDeclareNode& node)
{
	CodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	globalCode.add(seg);
}

void GlobalCodeGenVisitor::visit(DoubleDeclareNode& node)
{	
	CodeGenVisitor visitor(parser);
	node.visit(visitor);
	CodeSeg seg = visitor.getCode();
	globalCode.add(seg);
}
