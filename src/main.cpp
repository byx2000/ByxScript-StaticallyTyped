#include <iostream>
#include "ByxVM/Test/ByxVMTest.h"
#include "ByxLexer/Test/LexerTest.h"
#include "ByxParser/Test/ParserTest.h"
#include "ByxParser/ByxParser.h"

using namespace std;

int main()
{
	// �ڴ�й©���
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// ByxVM��Ԫ����
	ByxVMTest::Run();

	// Lexer��Ԫ����
	LexerTest::Run();

	// ByxParser��Ԫ����
	ByxParserTest::Run();

	return 0;
}