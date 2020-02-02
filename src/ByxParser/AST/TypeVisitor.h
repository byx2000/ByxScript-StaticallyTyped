#pragma once

#include "ASTVisitor.h"

class TypeVisitor : public ASTVisitor
{
private:
	virtual void visit(BinaryOpNode& node) override;
};