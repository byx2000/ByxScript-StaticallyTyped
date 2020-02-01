#include "ToStringVisitor.h"

using namespace std;

void ToStringVisitor::visit(ProgramNode& node)
{
	str += "Program:\n";
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
		str += "\n";
	}
}

void ToStringVisitor::visit(IntegerNode& node)
{
	str += to_string(node.val);
}

void ToStringVisitor::visit(DoubleNode& node)
{
	str += to_string(node.val);
}

void ToStringVisitor::visit(IntDeclareNode& node)
{
	str += "IntDec(";
	str += node.name;
	str += ",";
	node.expr->visit(*this);
	str += ")";
}

void ToStringVisitor::visit(DoubleDeclareNode& node)
{
	str += "DoubleDec(";
	str += node.name;
	str += ",";
	node.expr->visit(*this);
	str += ")";
}

void ToStringVisitor::visit(FunctionDeclareNode& node)
{
	str += "FuncDec(";
	str += node.name;
	str += ",";
	str += DataTypeToString(node.retType);
	str += ",";
	for (int i = 0; i < (int)node.paramType.size(); ++i)
	{
		str += DataTypeToString(node.paramType[i]);
		str += ",";
		str += node.paramName[i];
		str += ",";
	}
	str += "body(";
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(*this);
		if (i != (int)node.body.size() - 1)
		{
			str += ",";
		}
	}
	str += "))";
}

void ToStringVisitor::visit(CodeBlockNode& node)
{
	str += "block(";
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
		if (i != (int)node.stmts.size() - 1)
		{
			str += ",";
		}
	}
	str += ")";
}

void ToStringVisitor::visit(VarNode& node)
{
	str += node.name;
}

void ToStringVisitor::visit(VarAssignNode& node)
{
	str += "=(";
	str += node.name;
	str += ",";
	node.expr->visit(*this);
	str += ")";
}

void ToStringVisitor::visit(ReturnNode& node)
{
	str += "ret(";
	if (node.hasExpr)
	{
		node.expr->visit(*this);
	}
	str += ")";
}

void ToStringVisitor::visit(FunctionCallStmtNode& node)
{
	str += node.name;
	str += "(";
	for (int i = 0; i < (int)node.exprs.size(); ++i)
	{
		node.exprs[i]->visit(*this);
		if (i != (int)node.exprs.size() - 1)
		{
			str += ",";
		}
	}
	str += ")";
}

void ToStringVisitor::visit(FunctionCallExprNode& node)
{
	str += node.name;
	str += "(";
	for (int i = 0; i < (int)node.exprs.size(); ++i)
	{
		node.exprs[i]->visit(*this);
		if (i != (int)node.exprs.size() - 1)
		{
			str += ",";
		}
	}
	str += ")";
}

std::string ToStringVisitor::getString() const
{
	return str;
}