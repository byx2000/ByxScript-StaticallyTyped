#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../Common/DataType.h"
#include "../../ByxLexer/Token.h"
#include "../SymbolInfo.h"
#include "../FunctionInfo.h"

class ASTVisitor;

// 抽象语法树节点基类
class ASTNode
{
public:
	virtual void visit(ASTVisitor& visitor) = 0; // 访问者模式
	virtual ~ASTNode();

	int row() const;
	int col() const;

	Token token; // 原始词法单元
};

// 表达式节点基类
class Expression : public ASTNode
{
public:
	Expression();
	DataType dataType;
};

// 空语句
class EmptyNode : public ASTNode
{
public:
	virtual void visit(ASTVisitor& visitor) override;
};

// 程序
class ProgramNode : public ASTNode
{
public:
	ProgramNode(const std::vector<std::shared_ptr<ASTNode>>& stmts);
	virtual void visit(ASTVisitor& visitor) override;

	std::vector<std::shared_ptr<ASTNode>> stmts;
};

// 代码块
class CodeBlockNode : public ASTNode
{
public:
	CodeBlockNode(const std::vector<std::shared_ptr<ASTNode>>& stmts);
	virtual void visit(ASTVisitor& visitor) override;

	std::vector<std::shared_ptr<ASTNode>> stmts;
};

// 整型常量
class IntegerNode : public Expression
{
public:
	IntegerNode(int val);
	virtual void visit(ASTVisitor& visitor) override;

	int val;
};

// 浮点型常量
class DoubleNode : public Expression
{
public:
	DoubleNode(double val);
	virtual void visit(ASTVisitor& visitor) override;

	double val;
};

// 整型变量声明
class IntDeclareNode : public ASTNode
{
public:
	IntDeclareNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // 符号信息
};

// 浮点型变量声明
class DoubleDeclareNode : public ASTNode
{
public:
	DoubleDeclareNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // 符号信息
};

// 函数声明
class FunctionDeclareNode : public ASTNode
{
public:
	FunctionDeclareNode(DataType retType, const std::string& name, const std::vector<DataType>& paramType, const std::vector<std::string>& paramName, const std::vector<std::shared_ptr<ASTNode>>& body, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	DataType retType;
	std::string name;
	std::vector<DataType> paramType;
	std::vector<std::string> paramName;
	std::vector<std::shared_ptr<ASTNode>> body;
};

// 变量
class VarNode : public Expression
{
public:
	VarNode(const std::string& name, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	SymbolInfo symbol; // 符号信息
};

// 赋值
class VarAssignNode : public ASTNode
{
public:
	VarAssignNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // 符号信息
};

// 函数返回
class ReturnNode : public ASTNode
{
public:
	ReturnNode(bool hasExpr, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	bool hasExpr;
	std::shared_ptr<Expression> expr;
};

// 函数调用（语句）
class FunctionCallStmtNode : public ASTNode
{
public:
	FunctionCallStmtNode(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::vector<std::shared_ptr<Expression>> exprs;
};

//函数调用（表达式）
class FunctionCallExprNode : public Expression
{
public:
	FunctionCallExprNode(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::vector<std::shared_ptr<Expression>> exprs;
};

// if语句
class IfNode : public ASTNode
{
public:
	IfNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> tBranch, std::shared_ptr<ASTNode> fBranch, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::shared_ptr<Expression> cond;
	std::shared_ptr<ASTNode> tBranch;
	std::shared_ptr<ASTNode> fBranch;
};

// 二元表达式
class BinaryOpNode : public Expression
{
public:
	BinaryOpNode(int opType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	enum { Add, Sub, Mul, Div, Equ, NotEqu, Larger, Less, LargerEqu, LessEqu };
	int opType;
	std::shared_ptr<Expression> lhs, rhs;
};

// while语句
class WhileNode : public ASTNode
{
public:
	WhileNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> body, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::shared_ptr<Expression> cond;
	std::shared_ptr<ASTNode> body;
};

// break语句
class BreakNode : public ASTNode
{
public:
	BreakNode(const Token& token);
	virtual void visit(ASTVisitor& visitor) override;
};

// continue语句
class ContinueNode : public ASTNode
{
public:
	ContinueNode(const Token& token);
	virtual void visit(ASTVisitor& visitor) override;
};