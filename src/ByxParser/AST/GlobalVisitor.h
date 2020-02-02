#pragma once

#include "ASTVisitor.h"
#include "../../Common/CodeSeg.h"

#include <string>
#include <map>

class ByxParser;

class GlobalVisitor : public ASTVisitor
{
public:
	GlobalVisitor(ByxParser& parser);
	CodeSeg getInitCode();

private:	
	ByxParser& parser;
	int funcIndex;
	int varIndex;
	CodeSeg initCode;

	virtual void visit(ProgramNode& node) override;
	virtual void visit(IntDeclareNode& node) override;
	virtual void visit(DoubleDeclareNode& node) override;
};