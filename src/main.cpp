#include <iostream>
#include <string>
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

	try
	{
		ByxParser parser("function void main() { int i; if (1) { i = 100; } else { i = 200; } }");
		parser.parse();
		cout << parser.getASTString() << endl;
	}
	catch (ByxParser::ParseError err)
	{
		cout << err.getMsg() << endl;
		cout << err.getRow() << " " << err.getCol() << endl;
	}

	return 0;
}