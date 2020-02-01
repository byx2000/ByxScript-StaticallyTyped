#include "AST.h"
#include "../ScopeStack.h"
#include "../../Common/Code.h"
#include "ToStringVisitor.h"

#include <iostream>

using namespace std;

ASTNode::~ASTNode()
{

}

int ASTNode::row() const
{
	return token.row;
}

int ASTNode::col() const
{
	return token.col;
}

void EmptyNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

ProgramNode::ProgramNode(const std::vector<shared_ptr<ASTNode>>& stmts)
	: stmts(stmts)
{

}

void ProgramNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

CodeBlockNode::CodeBlockNode(const std::vector<shared_ptr<ASTNode>>& stmts)
	: stmts(stmts)
{

}

void CodeBlockNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

IntegerNode::IntegerNode(int val) : val(val)
{
	type = DataType::Integer;
}

void IntegerNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

DoubleNode::DoubleNode(double val) : val(val)
{
	type = DataType::Double;
}

void DoubleNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

IntDeclareNode::IntDeclareNode(const std::string& name, shared_ptr<Expression> expr, const Token& token)
	: name(name), expr(expr)
{
	this->token = token;
}

void IntDeclareNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

DoubleDeclareNode::DoubleDeclareNode(const std::string& name, shared_ptr<Expression> expr, const Token& token)
	: name(name), expr(expr)
{
	this->token = token;
}

void DoubleDeclareNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

FunctionDeclareNode::FunctionDeclareNode(DataType retType, const std::string& name, const std::vector<DataType>& paramType, const std::vector<std::string>& paramName, const std::vector<shared_ptr<ASTNode>>& body, const Token& token)
	: retType(retType), name(name), paramType(paramType), paramName(paramName), body(body)
{
	this->token = token;
}

void FunctionDeclareNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

VarNode::VarNode(const std::string& name, const Token& token)
	: name(name)
{
	this->token = token;
	type = DataType::Unknown;
}

void VarNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

VarAssignNode::VarAssignNode(const std::string& name, shared_ptr<Expression> expr, const Token& token)
	: name(name), expr(expr)
{
	this->token = token;
}

void VarAssignNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

ReturnNode::ReturnNode(bool hasExpr, shared_ptr<Expression> expr, const Token& token)
	: hasExpr(hasExpr), expr(expr)
{
	this->token = token;
}

void ReturnNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

FunctionCallStmtNode::FunctionCallStmtNode(const std::string& name, const std::vector<shared_ptr<Expression>>& exprs, const Token& token)
	: name(name), exprs(exprs)
{
	this->token = token;
}

void FunctionCallStmtNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

FunctionCallExprNode::FunctionCallExprNode(const std::string& name, const std::vector<shared_ptr<Expression>>& exprs, const Token& token)
	: name(name), exprs(exprs)
{
	this->token = token;
	type = DataType::Unknown;
}

void FunctionCallExprNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

IfNode::IfNode(std::shared_ptr<Expression> cond, std::shared_ptr<Expression> tBranck, std::shared_ptr<Expression> fBranch, const Token& token)
	: cond(cond), tBranch(tBranch), fBranch(fBranch)
{
	this->token = token;
}

void IfNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}
