#pragma once

#include "ASTVisitor.h"
#include "../../../Common/Instruction.h"

#include <string>
#include <map>

class ByxParser;

class GlobalVisitor : public ASTVisitor
{
public:
	GlobalVisitor(ByxParser& parser);
	std::vector<Instruction> getInitCode();

private:	
	ByxParser& parser;
	int funcIndex;
	int varIndex;
	std::vector<Instruction> initCode;

	virtual void visit(ProgramNode& node) override;
	virtual void visit(IntDeclareNode& node) override;
	virtual void visit(DoubleDeclareNode& node) override;
};