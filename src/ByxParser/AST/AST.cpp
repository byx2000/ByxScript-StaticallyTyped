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

Expression::Expression()
{
	dataType = DataType::Unknown;
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
	dataType = DataType::Integer;
}

void IntegerNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

DoubleNode::DoubleNode(double val) : val(val)
{
	dataType = DataType::Double;
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
	dataType = DataType::Unknown;
}

void FunctionCallExprNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

IfNode::IfNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> tBranch, std::shared_ptr<ASTNode> fBranch, const Token& token)
	: cond(cond), tBranch(tBranch), fBranch(fBranch)
{
	this->token = token;
}

void IfNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

BinaryOpNode::BinaryOpNode(int opType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, const Token& token)
	: opType(opType), lhs(lhs), rhs(rhs)
{
	this->token = token;
}

void BinaryOpNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

WhileNode::WhileNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> body, const Token& token)
	: cond(cond), body(body)
{
	this->token = token;
}

void WhileNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

BreakNode::BreakNode(const Token& token)
{
	this->token = token;
}

void BreakNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

ContinueNode::ContinueNode(const Token& token)
{
	this->token = token;
}

void ContinueNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

UnaryOpNode::UnaryOpNode(int opType, std::shared_ptr<Expression> expr, const Token& token)
	: opType(opType), expr(expr)
{
	this->token = token;
}

void UnaryOpNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}

ForNode::ForNode(std::shared_ptr<ASTNode> init, std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> update, std::shared_ptr<ASTNode> body, const Token& token)
	: init(init), cond(cond), update(update), body(body)
{
	this->token = token;
}

void ForNode::visit(ASTVisitor& visitor)
{
	visitor.visit(*this);
}
