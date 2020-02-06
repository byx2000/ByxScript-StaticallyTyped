#include <iostream>
#include <string>
#include "ByxVM/Test/ByxVMTest.h"
#include "ByxLexer/Test/LexerTest.h"
#include "ByxParser/Test/ParserTest.h"
#include "ByxParser/ByxParser.h"
#include "Common/Value.h"

using namespace std;

int main()
{
	// 内存泄漏检测
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ByxVM单元测试
	ByxVMTest::Run();

	// ByxLexer单元测试
	LexerTest::Run();

	// ByxParser单元测试
	ByxParserTest::Run();

	cout << endl;
	try
	{
		ByxParser parser("function void main() { int s = 0; for (int i = 1; i <= 100; i = i + 1) s = s + i; }");
		parser.parse();
		cout << parser.getASTString() << endl;
		cout << parser.getCode().toString() << endl;
		cout << parser.getFunctionTable().toString() << endl;
	}
	catch (ByxParser::ParseError err)
	{
		cout << err.getMsg() << endl;
		cout << err.getRow() << " " << err.getCol() << endl;
	}

	return 0;
}