#include "GlobalVisitor.h"
#include "../ByxParser.h"
#include "CodeGenVisitor.h"

using namespace std;

GlobalVisitor::GlobalVisitor(ByxParser& parser)
	: parser(parser)
{
	funcIndex = 1;
	varIndex = 0;
}

std::vector<Instruction> GlobalVisitor::getInitCode()
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
	CodeGenVisitor visitor(parser);
	node.visit(visitor);

	vector<Instruction> v = visitor.getCode();
	for (int i = 0; i < (int)v.size(); ++i)
	{
		initCode.push_back(v[i]);
	}
}

void GlobalVisitor::visit(DoubleDeclareNode& node)
{	
	CodeGenVisitor visitor(parser);
	node.visit(visitor);

	vector<Instruction> v = visitor.getCode();
	for (int i = 0; i < (int)v.size(); ++i)
	{
		initCode.push_back(v[i]);
	}
}
