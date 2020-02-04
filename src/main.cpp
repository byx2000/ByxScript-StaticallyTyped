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
	// �ڴ�й©���
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ByxVM��Ԫ����
	ByxVMTest::Run();

	// ByxLexer��Ԫ����
	LexerTest::Run();

	// ByxParser��Ԫ����
	ByxParserTest::Run();

	cout << endl;
	try
	{
		ByxParser parser("function void main() { while (1) { continue; } }");
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