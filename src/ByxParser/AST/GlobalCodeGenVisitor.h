#pragma once

#include "ASTVisitor.h"
#include "../../Common/CodeSeg.h"

#include <string>
#include <map>

class ByxParser;

class GlobalCodeGenVisitor : public ASTVisitor
{
public:
	GlobalCodeGenVisitor(ByxParser& parser);
	CodeSeg getGlobalCode();

private:	
	ByxParser& parser;
	int funcIndex;
	int varIndex;
	CodeSeg globalCode;

	virtual void visit(ProgramNode& node) override;
	virtual void visit(IntDeclareNode& node) override;
	virtual void visit(DoubleDeclareNode& node) override;
};