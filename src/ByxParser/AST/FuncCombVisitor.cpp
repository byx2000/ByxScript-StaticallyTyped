#include "FuncCombVisitor.h"
#include "CodeGenVisitor.h"
#include "../FunctionInfo.h"
#include "../ByxParser.h"

#include <iostream>

using namespace std;

FuncCombVisitor::FuncCombVisitor(ByxParser& parser)
	: parser(parser)
{

}

CodeSeg FuncCombVisitor::getCode() const
{
	return codeSeg;
}

void FuncCombVisitor::visit(ProgramNode& node)
{
	for (int i = 0; i < (int)node.stmts.size(); ++i)
	{
		node.stmts[i]->visit(*this);
	}
}

void FuncCombVisitor::visit(FunctionDeclareNode& node)
{
	// 获取函数信息
	FunctionInfo info = parser.functionInfo[node.name];

	// 设置函数起始地址
	parser.functionTable.setAddr(info.index, codeSeg.getSize());

	// 为main函数添加全局变量初始化代码
	if (node.name == "main")
	{
		codeSeg.add(parser.initCode);
	}

	// 生成参数读取指令
	int cnt = 0;
	for (int i = 0; i < (int)node.paramName.size(); ++i)
	{
		if (node.paramType[i] == DataType::Integer)
		{
			codeSeg.add(Opcode::istore, cnt);
		}
		else if (node.paramType[i] == DataType::Double)
		{
			codeSeg.add(Opcode::dstore, cnt);
		}
		cnt++;
	}

	// 生成函数体代码
	CodeGenVisitor visitor(parser, node.name);
	//node.visit(visitor);
	
	for (int i = 0; i < (int)node.body.size(); ++i)
	{
		node.body[i]->visit(visitor);
	}

	CodeSeg seg = visitor.getCode();
	codeSeg.add(seg);

	// 添加ret指令（该处可能会生成重复的ret指令）
	codeSeg.add(Opcode::ret);
}
