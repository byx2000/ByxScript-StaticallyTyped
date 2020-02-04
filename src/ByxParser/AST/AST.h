#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../Common/DataType.h"
#include "../../ByxLexer/Token.h"
#include "../SymbolInfo.h"
#include "../FunctionInfo.h"

class ASTVisitor;

// �����﷨���ڵ����
class ASTNode
{
public:
	virtual void visit(ASTVisitor& visitor) = 0; // ������ģʽ
	virtual ~ASTNode();

	int row() const;
	int col() const;

	Token token; // ԭʼ�ʷ���Ԫ
};

// ���ʽ�ڵ����
class Expression : public ASTNode
{
public:
	Expression();
	DataType dataType;
};

// �����
class EmptyNode : public ASTNode
{
public:
	virtual void visit(ASTVisitor& visitor) override;
};

// ����
class ProgramNode : public ASTNode
{
public:
	ProgramNode(const std::vector<std::shared_ptr<ASTNode>>& stmts);
	virtual void visit(ASTVisitor& visitor) override;

	std::vector<std::shared_ptr<ASTNode>> stmts;
};

// �����
class CodeBlockNode : public ASTNode
{
public:
	CodeBlockNode(const std::vector<std::shared_ptr<ASTNode>>& stmts);
	virtual void visit(ASTVisitor& visitor) override;

	std::vector<std::shared_ptr<ASTNode>> stmts;
};

// ���ͳ���
class IntegerNode : public Expression
{
public:
	IntegerNode(int val);
	virtual void visit(ASTVisitor& visitor) override;

	int val;
};

// �����ͳ���
class DoubleNode : public Expression
{
public:
	DoubleNode(double val);
	virtual void visit(ASTVisitor& visitor) override;

	double val;
};

// ���ͱ�������
class IntDeclareNode : public ASTNode
{
public:
	IntDeclareNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // ������Ϣ
};

// �����ͱ�������
class DoubleDeclareNode : public ASTNode
{
public:
	DoubleDeclareNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // ������Ϣ
};

// ��������
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

// ����
class VarNode : public Expression
{
public:
	VarNode(const std::string& name, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	SymbolInfo symbol; // ������Ϣ
};

// ��ֵ
class VarAssignNode : public ASTNode
{
public:
	VarAssignNode(const std::string& name, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::shared_ptr<Expression> expr;
	SymbolInfo symbol; // ������Ϣ
};

// ��������
class ReturnNode : public ASTNode
{
public:
	ReturnNode(bool hasExpr, std::shared_ptr<Expression> expr, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	bool hasExpr;
	std::shared_ptr<Expression> expr;
};

// �������ã���䣩
class FunctionCallStmtNode : public ASTNode
{
public:
	FunctionCallStmtNode(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::vector<std::shared_ptr<Expression>> exprs;
};

//�������ã����ʽ��
class FunctionCallExprNode : public Expression
{
public:
	FunctionCallExprNode(const std::string& name, const std::vector<std::shared_ptr<Expression>>& exprs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::string name;
	std::vector<std::shared_ptr<Expression>> exprs;
};

// if���
class IfNode : public ASTNode
{
public:
	IfNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> tBranch, std::shared_ptr<ASTNode> fBranch, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::shared_ptr<Expression> cond;
	std::shared_ptr<ASTNode> tBranch;
	std::shared_ptr<ASTNode> fBranch;
};

// ��Ԫ���ʽ
class BinaryOpNode : public Expression
{
public:
	BinaryOpNode(int opType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	enum { Add, Sub, Mul, Div, Equ, NotEqu, Larger, Less, LargerEqu, LessEqu };
	int opType;
	std::shared_ptr<Expression> lhs, rhs;
};

// while���
class WhileNode : public ASTNode
{
public:
	WhileNode(std::shared_ptr<Expression> cond, std::shared_ptr<ASTNode> body, const Token& token);
	virtual void visit(ASTVisitor& visitor) override;

	std::shared_ptr<Expression> cond;
	std::shared_ptr<ASTNode> body;
};

// break���
class BreakNode : public ASTNode
{
public:
	BreakNode(const Token& token);
	virtual void visit(ASTVisitor& visitor) override;
};

// continue���
class ContinueNode : public ASTNode
{
public:
	ContinueNode(const Token& token);
	virtual void visit(ASTVisitor& visitor) override;
};