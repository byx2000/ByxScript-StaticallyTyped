#pragma once

#include "AST/AST.h"
#include "../ByxLexer/ByxLexer.h"
#include "ScopeStack.h"
#include "../Common/Code.h"
#include "../Common/FunctionTable.h"
#include "AST/SymbolVisitor.h"
#include "FunctionInfo.h"
#include "AST/FuncCodeGenVisitor.h"

#include <memory>

class ByxParser
{
	friend class CodeGenVisitor;
	friend class GlobalVisitor;
	friend class SymbolVisitor;
	friend class FuncCodeGenVisitor;
	friend class FuncCombVisitor;
public:
	ByxParser(const std::string& input);
	ByxParser& parse();
	std::string getASTString();
	FunctionTable getFunctionTable() const;
	Code getCode() const;
	int getGlobalSpace() const;

	// 异常
	class ParseError
	{
	public:
		ParseError(const std::string& msg, int row, int col);
		std::string getMsg() const;
		int getRow() const;
		int getCol() const;

	private:
		std::string msg;
		int row;
		int col;
	};

private:
	std::string input;
	ByxLexer lexer;
	std::shared_ptr<ASTNode> ast;
	Code code;

	FunctionTable functionTable; // 函数表
	CodeSeg initCode; // 初始化全局变量的代码
	std::map<std::string, FunctionInfo> functionInfo; // 函数信息

	int globalSpace; // 全局变量空间大小

	std::shared_ptr<ASTNode> parseProgram();
	std::shared_ptr<ASTNode> parseStatement();
	std::shared_ptr<ASTNode> parseFunctionDeclare();
	std::shared_ptr<ASTNode> parseIntDeclare();
	std::shared_ptr<ASTNode> parseVarAssign();
	std::shared_ptr<ASTNode> parseDoubleDeclare();
	std::shared_ptr<ASTNode> parseReturn();
	std::shared_ptr<ASTNode> parseCodeBlock();
	std::shared_ptr<ASTNode> parseFunctionCallStmt();
	std::shared_ptr<ASTNode> parseIf();
	std::shared_ptr<ASTNode> parseWhile();
	std::shared_ptr<Expression> parseExpr();
	std::shared_ptr<Expression> parseCmpExpr();
	std::shared_ptr<Expression> parseArithExpr();
	std::shared_ptr<Expression> parseTerm();
	std::shared_ptr<Expression> parseFactor();
	std::shared_ptr<Expression> parseFunctionCallExpr();
};