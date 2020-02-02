#include "ByxParser.h"
#include "AST/GlobalVisitor.h"
#include "AST/CodeGenVisitor.h"
#include "AST/ToStringVisitor.h"

#include <sstream>
#include <iostream>

using namespace std;

ByxParser::ParseError::ParseError(const std::string& msg, int row, int col)
	: msg(msg), row(row), col(col)
{
	
}

std::string ByxParser::ParseError::getMsg() const
{
	return msg;
}

int ByxParser::ParseError::getRow() const
{
	return row;
}

int ByxParser::ParseError::getCol() const
{
	return col;
}

static int StrToInt(const std::string& str)
{
	stringstream ss(str);
	int val;
	ss >> val;
	return val;
}

static double StrToDouble(const std::string& str)
{
	stringstream ss(str);
	double val;
	ss >> val;
	return val;
}

ByxParser::ByxParser(const std::string& input)
	: input(input), lexer(input)
{
	ast = NULL;
	globalSpace = 0;
}

ByxParser& ByxParser::parse()
{
	// �ʷ�����
	lexer.lex();

	// ���������﷨��
	ast = parseProgram();

	// ��һ�������ɨ�躯����ȫ�ֱ������ֲ������ȷ��ţ������������Ϣ
	SymbolVisitor symbolVisitor;
	ast->visit(symbolVisitor);
	globalSpace = symbolVisitor.getGlobalSpace(); // ȫ�ֱ����ռ�
	functionTable = symbolVisitor.getFunctionTable(); // ������
	functionInfo = symbolVisitor.getFunctionInfo();

	// �ڶ������������ȫ�ֳ�ʼ������
	GlobalVisitor globalVisitor(*this);
	ast->visit(globalVisitor);
	initCode = globalVisitor.getInitCode();

	// ������������������д���
	CodeGenVisitor codeGenVisitor(*this);
	ast->visit(codeGenVisitor);
	code = Code(codeGenVisitor.getCodeSeg());

	//cout << code.toString() << endl;
	//cout << functionTable.toString() << endl;

	return *this;
}

std::string ByxParser::getASTString()
{
	ToStringVisitor visitor;
	ast->visit(visitor);
	return visitor.getString();
}

FunctionTable ByxParser::getFunctionTable() const
{
	return functionTable;
}

Code ByxParser::getCode() const
{
	return code;
}

int ByxParser::getGlobalSpace() const
{
	return globalSpace;
}

shared_ptr<ASTNode> ByxParser::parseProgram()
{
	vector<shared_ptr<ASTNode>> stmts;
	while (lexer.nextType() != TokenType::End)
	{
		if (lexer.nextType() == TokenType::Keyword)
		{
			if (lexer.nextVal() == "int") // ����ȫ�ֱ�������
			{
				stmts.push_back(parseIntDeclare());
			}
			else if (lexer.nextVal() == "double") // ������ȫ�ֱ�������
			{
				stmts.push_back(parseDoubleDeclare());
			}
			else if (lexer.nextVal() == "function") // ��������
			{
				stmts.push_back(parseFunction());
			}
			else // ����
			{
				throw ParseError("Global scope only allow function declare and var declare.", lexer.row(), lexer.col());
			}
		}
		else // ����
		{
			throw ParseError("Global scope only allow function declare and var declare.", lexer.row(), lexer.col());
		}
	}
	
	return make_shared<ProgramNode>(stmts);
}

shared_ptr<ASTNode> ByxParser::parseStatement()
{
	Token token = lexer.peek();
	if (token.type == TokenType::Keyword)
	{
		// ���ͱ�������
		if (token.val == "int")
		{
			return parseIntDeclare();
		}
		// �����ͱ�������
		else if (token.val == "double")
		{
			return parseDoubleDeclare();
		}
		// ��������
		else if (token.val == "return")
		{
			return parseReturn();
		}
		// if
		else if (token.val == "if")
		{
			return parseIf();
		}
		// ����
		else
		{
			throw ParseError(string("Unexpected token: '") + token.val + "'", token.row, token.col);
		}
	}
	// ���飨��������
	else if (token.type == TokenType::OpenBrace)
	{
		return parseCodeBlock();
	}
	// ������ֵ��������
	else if (token.type == TokenType::Ident)
	{
		lexer.next();
		// ������ֵ
		if (lexer.nextType() == TokenType::Assign)
		{
			lexer.back();
			return parseVarAssign();
		}
		// �����������
		else
		{
			lexer.back();
			return parseFunctionCallStmt();
		}
	}
	// ����
	else
	{
		throw ParseError(string("Unexpected token: '") + token.val + "'", token.row, token.col);
	}
}

shared_ptr<ASTNode> ByxParser::parseFunction()
{
	// ��ȡfunction�ؼ���
	Token t = lexer.next();

	// ��ȡ��������
	Token token = lexer.read(TokenType::Keyword);
	DataType retType = DataType::Void;
	if (token.val == "int")
	{
		retType = DataType::Integer;
	}
	else if (token.val == "double")
	{
		retType = DataType::Double;
	}

	// ��ȡ������
	string name = lexer.read(TokenType::Ident).val;

	//��ȡ�����б�
	vector<DataType> paramType;
	vector<string> paramName;
	lexer.read(TokenType::OpenBracket);
	while (lexer.nextType() != TokenType::CloseBracket)
	{
		// ��ȡ����
		token = lexer.next();
		if (token.type != TokenType::Keyword)
		{
			throw ParseError("Type specification expected.", token.row, token.col);
		}

		if (token.val == "int")
		{
			paramType.push_back(DataType::Integer);
		}
		else if (token.val == "double")
		{
			paramType.push_back(DataType::Double);
		}
		else if (token.val == "void")
		{
			paramType.push_back(DataType::Void);
		}
		else
		{
			throw ParseError("Type specification expected.", token.row, token.col);
		}

		// ��ȡ������
		paramName.push_back(lexer.read(TokenType::Ident).val);

		if (lexer.nextType() == TokenType::CloseBracket)
		{
			break;
		}
		else
		{
			lexer.read(TokenType::Comma);
		}
	}
	lexer.next();

	// ��ȡ������
	vector<shared_ptr<ASTNode>> body;
	lexer.read(TokenType::OpenBrace);
	while (lexer.nextType() != TokenType::CloseBrace)
	{
		body.push_back(parseStatement());
	}
	lexer.next();

	// ���캯������ڵ�
	return make_shared<FunctionDeclareNode>(retType, name, paramType, paramName, body, t);
}

shared_ptr<ASTNode> ByxParser::parseIntDeclare()
{
	// ��ȡint�ؼ���
	Token token = lexer.next();

	// ��ȡ������
	string name = lexer.read(TokenType::Ident).val;

	// ��ȡ��ʼ�����ʽ
	shared_ptr<Expression> expr;
	if (lexer.nextType() != TokenType::Semicolon)
	{
		token = lexer.read(TokenType::Assign);
		expr = parseExpr();
	}
	else
	{
		expr = make_shared<IntegerNode>(0);
	}
	
	// ��ȡ�ֺ�
	lexer.read(TokenType::Semicolon);

	// �������ͱ��������ڵ�
	return make_shared<IntDeclareNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseVarAssign()
{
	// ��ȡ������
	string name = lexer.next().val;

	// ��ȡ��ֵ��
	Token token = lexer.read(TokenType::Assign);

	// ��ȡ���ʽ
	shared_ptr<Expression> expr = parseExpr();

	// ��ȡ�ֺ�
	lexer.read(TokenType::Semicolon);

	// ���츳ֵ�ڵ�
	return make_shared<VarAssignNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseDoubleDeclare()
{
	// ��ȡdouble�ؼ���
	Token token = lexer.next();

	// ��ȡ������
	string name = lexer.read(TokenType::Ident).val;

	// ��ȡ��ʼ�����ʽ
	shared_ptr<Expression> expr;
	if (lexer.nextType() != TokenType::Semicolon)
	{
		token = lexer.read(TokenType::Assign);
		expr = parseExpr();
	}
	else
	{
		expr = make_shared<DoubleNode>(0.0);
	}

	// ��ȡ�ֺ�
	lexer.read(TokenType::Semicolon);

	// �������ͱ��������ڵ�
	return make_shared<DoubleDeclareNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseReturn()
{
	// ��ȡreturn�ؼ���
	Token token = lexer.next();

	shared_ptr<Expression> expr = NULL;
	if (lexer.nextType() != TokenType::Semicolon)
	{
		expr = parseExpr();
		lexer.read(TokenType::Semicolon);
		return make_shared<ReturnNode>(true, expr, token);
	}
	else
	{
		lexer.next();
		return make_shared<ReturnNode>(false, expr, token);
	}
}

shared_ptr<ASTNode> ByxParser::parseCodeBlock()
{
	// ��ȡ�������
	lexer.next();

	// ��ȡ�������
	vector<shared_ptr<ASTNode>> stmts;
	while (lexer.nextType() != TokenType::CloseBrace)
	{
		stmts.push_back(parseStatement());
	}

	// ��ȡ�Ҵ�����
	lexer.next();

	// ��������ڵ�
	return make_shared<CodeBlockNode>(stmts);
}

shared_ptr<ASTNode> ByxParser::parseFunctionCallStmt()
{
	// ��ȡ������
	Token token = lexer.next();
	string name = token.val;

	// ��ȡ������
	lexer.read(TokenType::OpenBracket);

	// ��ȡ�����б�
	vector<shared_ptr<Expression>> exprs;
	while (lexer.nextType() != TokenType::CloseBracket)
	{
		exprs.push_back(parseExpr());
		if (lexer.nextType() == TokenType::CloseBracket)
		{
			break;
		}
		lexer.read(TokenType::Comma);
	}
	lexer.next();

	// ��ȡ�ֺ�
	lexer.read(TokenType::Semicolon);

	// ���캯���������ڵ�
	return make_shared<FunctionCallStmtNode>(name, exprs, token);
}

std::shared_ptr<ASTNode> ByxParser::parseIf()
{
	// ��ȡif�ؼ���
	Token token = lexer.next();

	// ��ȡ������
	lexer.read(TokenType::OpenBracket);

	// ��ȡ���ʽ
	shared_ptr<Expression> cond = parseExpr();

	// ��ȡ������
	lexer.read(TokenType::CloseBracket);

	// ��ȡtrue branch
	shared_ptr<ASTNode> tBranch = parseStatement();

	// ��ȡfalse branch
	shared_ptr<ASTNode> fBranch = make_shared<EmptyNode>();
	if (lexer.nextType() == TokenType::Keyword && lexer.nextVal() == "else")
	{
		lexer.next();
		fBranch = parseStatement();
	}

	// ����if�ڵ�
	return make_shared<IfNode>(cond, tBranch, fBranch, token);
}

shared_ptr<Expression> ByxParser::parseExpr()
{
	return parseCmpExpr();
}

shared_ptr<Expression> ByxParser::parseCmpExpr()
{
	return parseTerm();
}

shared_ptr<Expression> ByxParser::parseTerm()
{
	return parseFactor();
}

shared_ptr<Expression> ByxParser::parseFactor()
{
	Token token = lexer.next();
	if (token.type == TokenType::Integer) // ���ͳ���
	{
		return make_shared<IntegerNode>(StrToInt(token.val));
	}
	else if (token.type == TokenType::Double) // �����ͳ���
	{
		return make_shared<DoubleNode>(StrToDouble(token.val));
	}
	else if (token.type == TokenType::Ident) // ������������
	{
		if (lexer.nextType() == TokenType::OpenBracket) // ��������
		{
			lexer.back();
			return parseFunctionCallExpr();
		}
		else // ����
		{
			return make_shared<VarNode>(token.val, token);
		}
	}
	else
	{
		throw ParseError(string("Unexpected token: ") + token.toString(), token.row, token.col);
	}
}

shared_ptr<Expression> ByxParser::parseFunctionCallExpr()
{
	// ��ȡ������
	Token token = lexer.next();
	string name = token.val;

	// ��ȡ������
	lexer.read(TokenType::OpenBracket);

	// ��ȡ�����б�
	vector<shared_ptr<Expression>> exprs;
	while (lexer.nextType() != TokenType::CloseBracket)
	{
		exprs.push_back(parseExpr());
		if (lexer.nextType() == TokenType::CloseBracket)
		{
			break;
		}
		lexer.read(TokenType::Comma);
	}
	lexer.next();

	// ���캯�����ñ��ʽ�ڵ�
	return make_shared<FunctionCallExprNode>(name, exprs, token);
}
