#include <iostream>
#include "ByxVM/Test/ByxVMTest.h"
#include "ByxLexer/Test/LexerTest.h"
#include "ByxParser/Test/ParserTest.h"
#include "ByxParser/ByxParser.h"

using namespace std;

int main()
{
	// 内存泄漏检测
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ByxVM单元测试
	ByxVMTest::Run();

	// Lexer单元测试
	LexerTest::Run();

	// ByxParser单元测试
	ByxParserTest::Run();

	return 0;
}