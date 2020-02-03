#pragma once

#include "ASTVisitor.h"
#include "../../Common/CodeSeg.h"

class ByxParser;

class FuncCombVisitor : public ASTVisitor
{
public:
	FuncCombVisitor(ByxParser& parser);
	CodeSeg getCode() const;

private:
	ByxParser& parser;
	CodeSeg codeSeg;

	virtual void visit(ProgramNode& node) override;
	virtual void visit(FunctionDeclareNode& node) override;
};