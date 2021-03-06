#include "ByxParser.h"
#include "AST/GlobalCodeGenVisitor.h"
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
	parsingForHeader = false;
}

ByxParser& ByxParser::parse()
{
	// 词法分析
	lexer.lex();

	// 构建抽象语法树
	ast = parseProgram();

	// 第一遍遍历：扫描函数、全局变量、局部变量等符号，记录表达式类型，并整理相关信息
	SymbolVisitor symbolVisitor;
	ast->visit(symbolVisitor);
	globalSpace = symbolVisitor.getGlobalSpace(); // 全局变量空间
	functionTable = symbolVisitor.getFunctionTable(); // 函数表
	functionInfo = symbolVisitor.getFunctionInfo();

	// 第二遍遍历：生成全局初始化代码
	GlobalCodeGenVisitor globalVisitor(*this);
	ast->visit(globalVisitor);
	globalCode = globalVisitor.getGlobalCode();

	// 第三遍遍历：生成所有代码
	CodeGenVisitor codeGenVisitor(*this);
	ast->visit(codeGenVisitor);
	code = Code(codeGenVisitor.getCode());

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
			if (lexer.nextVal() == "int") // 整形全局变量声明
			{
				stmts.push_back(parseIntDeclare());
			}
			else if (lexer.nextVal() == "double") // 浮点型全局变量声明
			{
				stmts.push_back(parseDoubleDeclare());
			}
			else if (lexer.nextVal() == "function") // 函数声明
			{
				stmts.push_back(parseFunctionDeclare());
			}
			else // 出错
			{
				throw ParseError("Global scope only allow function declare and var declare.", lexer.row(), lexer.col());
			}
		}
		else // 出错
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
		// 整型变量声明
		if (token.val == "int")
		{
			return parseIntDeclare();
		}
		// 浮点型变量声明
		else if (token.val == "double")
		{
			return parseDoubleDeclare();
		}
		// 函数返回
		else if (token.val == "return")
		{
			if (parsingForHeader)
			{
				throw ByxParser::ParseError("Return statement cannot appear in for header.", token.row, token.col);
			}

			return parseReturn();
		}
		// if
		else if (token.val == "if")
		{
			if (parsingForHeader)
			{
				throw ByxParser::ParseError("If statement cannot appear in for header.", token.row, token.col);
			}

			return parseIf();
		}
		// while
		else if (token.val == "while")
		{
			if (parsingForHeader)
			{
				throw ByxParser::ParseError("While statement cannot appear in for header.", token.row, token.col);
			}

			return parseWhile();
		}
		// break
		else if (token.val == "break")
		{
			if (parsingForHeader)
			{
				throw ByxParser::ParseError("Break statement cannot appear in for header.", token.row, token.col);
			}

			Token t = lexer.next();
			lexer.read(TokenType::Semicolon);
			return make_shared<BreakNode>(t);
		}
		// continue
		else if (token.val == "continue")
		{
			if (parsingForHeader)
			{
				throw ByxParser::ParseError("Continue statement cannot appear in for header.", token.row, token.col);
			}

			Token t = lexer.next();
			lexer.read(TokenType::Semicolon);
			return make_shared<ContinueNode>(t);
		}
		// for
		else if (token.val == "for")
		{
			return parseFor();
		}
		// 出错
		else
		{
			throw ParseError(string("Unexpected token: '") + token.val + "'", token.row, token.col);
		}
	}
	// 语句块
	else if (token.type == TokenType::OpenBrace)
	{
		return parseCodeBlock();
	}
	// 空语句
	else if (token.type == TokenType::Semicolon)
	{
		lexer.next();
		return make_shared<EmptyNode>();
	}
	// 变量赋值或函数调用
	else if (token.type == TokenType::Ident)
	{
		lexer.next();
		// 变量赋值
		if (lexer.nextType() == TokenType::Assign)
		{
			lexer.back();
			return parseVarAssign();
		}
		// 函数调用语句
		else
		{
			lexer.back();
			return parseFunctionCallStmt();
		}
	}
	// 出错
	else
	{
		throw ParseError(string("Unexpected token: '") + token.val + "'", token.row, token.col);
	}
}

shared_ptr<ASTNode> ByxParser::parseFunctionDeclare()
{
	// 读取function关键字
	Token t = lexer.next();

	// 读取返回类型
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

	// 读取函数名
	string name = lexer.read(TokenType::Ident).val;

	//读取参数列表
	vector<DataType> paramType;
	vector<string> paramName;
	lexer.read(TokenType::OpenBracket);
	while (lexer.nextType() != TokenType::CloseBracket)
	{
		// 读取类型
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

		// 读取参数名
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

	// 读取函数体
	vector<shared_ptr<ASTNode>> body;
	lexer.read(TokenType::OpenBrace);
	while (lexer.nextType() != TokenType::CloseBrace)
	{
		body.push_back(parseStatement());
	}
	lexer.next();

	// 构造函数定义节点
	return make_shared<FunctionDeclareNode>(retType, name, paramType, paramName, body, t);
}

shared_ptr<ASTNode> ByxParser::parseIntDeclare()
{
	// 读取int关键字
	Token token = lexer.next();

	// 读取变量名
	string name = lexer.read(TokenType::Ident).val;

	// 读取初始化表达式
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

	// 读取分号
	readSemicolon();

	// 构造整型变量声明节点
	return make_shared<IntDeclareNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseVarAssign()
{
	// 读取变量名
	string name = lexer.next().val;

	// 读取赋值符
	Token token = lexer.read(TokenType::Assign);

	// 读取表达式
	shared_ptr<Expression> expr = parseExpr();

	// 读取分号
	readSemicolon();

	// 构造赋值节点
	return make_shared<VarAssignNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseDoubleDeclare()
{
	// 读取double关键字
	Token token = lexer.next();

	// 读取变量名
	string name = lexer.read(TokenType::Ident).val;

	// 读取初始化表达式
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

	// 读取分号
	readSemicolon();

	// 构造整型变量声明节点
	return make_shared<DoubleDeclareNode>(name, expr, token);
}

shared_ptr<ASTNode> ByxParser::parseReturn()
{
	// 读取return关键字
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
	// 读取左大括号
	lexer.next();

	// 读取语句序列
	vector<shared_ptr<ASTNode>> stmts;
	while (lexer.nextType() != TokenType::CloseBrace)
	{
		stmts.push_back(parseStatement());
	}

	// 读取右大括号
	lexer.next();

	// 构造语句块节点
	return make_shared<CodeBlockNode>(stmts);
}

shared_ptr<ASTNode> ByxParser::parseFunctionCallStmt()
{
	// 读取函数名
	Token token = lexer.next();
	string name = token.val;

	// 读取左括号
	lexer.read(TokenType::OpenBracket);

	// 读取参数列表
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

	// 读取分号
	readSemicolon();

	// 构造函数调用语句节点
	return make_shared<FunctionCallStmtNode>(name, exprs, token);
}

std::shared_ptr<ASTNode> ByxParser::parseIf()
{
	// 读取if关键字
	Token token = lexer.next();

	// 读取左括号
	lexer.read(TokenType::OpenBracket);

	// 读取表达式
	shared_ptr<Expression> cond = parseExpr();

	// 读取右括号
	lexer.read(TokenType::CloseBracket);

	// 读取true branch
	shared_ptr<ASTNode> tBranch = parseStatement();

	// 读取false branch
	shared_ptr<ASTNode> fBranch = make_shared<EmptyNode>();
	if (lexer.nextType() == TokenType::Keyword && lexer.nextVal() == "else")
	{
		lexer.next();
		fBranch = parseStatement();
	}

	// 构造if节点
	return make_shared<IfNode>(cond, tBranch, fBranch, token);
}

std::shared_ptr<ASTNode> ByxParser::parseWhile()
{
	// 读取while关键字
	Token token = lexer.next();

	// 读取左括号
	lexer.read(TokenType::OpenBracket);

	// 读取条件表达式
	shared_ptr<Expression> cond = parseExpr();

	// 读取右括号
	lexer.read(TokenType::CloseBracket);

	// 读取循环体
	shared_ptr<ASTNode> body = parseStatement();

	// 构造while循环节点
	return make_shared<WhileNode>(cond, body, token);
}

std::shared_ptr<ASTNode> ByxParser::parseFor()
{
	// 读取for关键字
	Token token = lexer.next();

	// 读取左括号
	lexer.read(TokenType::OpenBracket);

	parsingForHeader = true;

	// 读取初始化语句
	shared_ptr<ASTNode> init = parseStatement();
	
	// 读取分号
	lexer.read(TokenType::Semicolon);

	// 读取条件表达式
	shared_ptr<Expression> cond = parseExpr();

	// 读取分号
	lexer.read(TokenType::Semicolon);

	// 读取更新语句
	shared_ptr<ASTNode> update = parseStatement();

	parsingForHeader = false;

	// 读取右括号
	lexer.read(TokenType::CloseBracket);

	// 读取循环体
	shared_ptr<ASTNode> body = parseStatement();

	// 构造for节点
	return make_shared<ForNode>(init, cond, update, body, token);
}

shared_ptr<Expression> ByxParser::parseExpr()
{
	//return parseCmpExpr();
	shared_ptr<Expression> res = parseCmpExpr();
	while (lexer.nextType() == TokenType::And || lexer.nextType() == TokenType::Or)
	{
		Token token = lexer.next();
		if (token.type == TokenType::And)
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::And, res, parseCmpExpr(), token);
		}
		else
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Or, res, parseCmpExpr(), token);
		}
	}
	return res;
}

shared_ptr<Expression> ByxParser::parseCmpExpr()
{
	shared_ptr<Expression> res = parseArithExpr();
	if (lexer.nextType() == TokenType::Equal)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::Equ, res, parseArithExpr(), token);
	}
	else if (lexer.nextType() == TokenType::NotEqual)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::NotEqu, res, parseArithExpr(), token);
	}
	else if (lexer.nextType() == TokenType::LargerThan)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::Larger, res, parseArithExpr(), token);
	}
	else if (lexer.nextType() == TokenType::LessThan)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::Less, res, parseArithExpr(), token);
	}
	else if (lexer.nextType() == TokenType::LargerEqualThan)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::LargerEqu, res, parseArithExpr(), token);
	}
	else if (lexer.nextType() == TokenType::LessEqualThan)
	{
		Token token = lexer.next();
		return make_shared<BinaryOpNode>(BinaryOpNode::LessEqu, res, parseArithExpr(), token);
	}
	else
	{
		return res;
	}
}

std::shared_ptr<Expression> ByxParser::parseArithExpr()
{
	shared_ptr<Expression> res = parseTerm();
	while (lexer.nextType() == TokenType::Add || lexer.nextType() == TokenType::Sub)
	{
		Token token = lexer.next();
		if (token.type == TokenType::Add)
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Add, res, parseTerm(), token);
		}
		else
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Sub, res, parseTerm(), token);
		}
	}
	return res;
}

shared_ptr<Expression> ByxParser::parseTerm()
{
	shared_ptr<Expression> res = parseFactor();
	while (lexer.nextType() == TokenType::Mul || lexer.nextType() == TokenType::Div || lexer.nextType() == TokenType::Rem)
	{
		Token token = lexer.next();
		if (token.type == TokenType::Mul)
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Mul, res, parseFactor(), token);
		}
		else if (token.type == TokenType::Div)
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Div, res, parseFactor(), token);
		}
		else
		{
			res = make_shared<BinaryOpNode>(BinaryOpNode::Rem, res, parseFactor(), token);
		}
	}
	return res;
}

shared_ptr<Expression> ByxParser::parseFactor()
{
	Token token = lexer.next();
	if (token.type == TokenType::Integer) // 整型常量
	{
		return make_shared<IntegerNode>(StrToInt(token.val));
	}
	else if (token.type == TokenType::Double) // 浮点型常量
	{
		return make_shared<DoubleNode>(StrToDouble(token.val));
	}
	else if (token.type == TokenType::OpenBracket) // 括号表达式
	{
		shared_ptr<Expression> res = parseExpr();
		lexer.read(TokenType::CloseBracket);
		return res;
	}
	else if (token.type == TokenType::Add) // 正号
	{
		return make_shared<UnaryOpNode>(UnaryOpNode::Pos, parseFactor(), token);
	}
	else if (token.type == TokenType::Sub) // 负号
	{
		return make_shared<UnaryOpNode>(UnaryOpNode::Neg, parseFactor(), token);
	}
	else if (token.type == TokenType::Not) // 非运算
	{
		return make_shared<UnaryOpNode>(UnaryOpNode::Not, parseFactor(), token);
	}
	else if (token.type == TokenType::Ident) // 变量或函数调用
	{
		if (lexer.nextType() == TokenType::OpenBracket) // 函数调用
		{
			lexer.back();
			return parseFunctionCallExpr();
		}
		else // 变量
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
	// 读取函数名
	Token token = lexer.next();
	string name = token.val;

	// 读取左括号
	lexer.read(TokenType::OpenBracket);

	// 读取参数列表
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

	// 构造函数调用表达式节点
	return make_shared<FunctionCallExprNode>(name, exprs, token);
}

void ByxParser::readSemicolon()
{
	if (!parsingForHeader)
	{
		lexer.read(TokenType::Semicolon);
	}
}
