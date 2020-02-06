#include "ParserTest.h"
#include "../ByxParser.h"
#include "../../Common/TestUtil.h"
#include "../../ByxVM/ByxVM.h"

#include <string>
#include <fstream>
#include <sstream>

using namespace std;

void ByxParserTest::Run()
{
	StopWatch watch;
	watch.begin();

	// 代码生成测试
	int numCases = 23;
	for (int i = 1; i <= numCases; ++i)
	{
		string path = "src/ByxParser/Test/TestCase/CodeGenTest/";
		CheckCase(path + to_string(i) + ".in", path + to_string(i) + ".out");
		cout << i << " ";
	}
	cout << endl;

	// 代码生成异常测试
	int numErrorCase = 37;
	for (int i = 1; i <= numErrorCase; ++i)
	{
		string path = "src/ByxParser/Test/TestCase/CodeGenTest/Error/";
		CheckErrorCase(path + to_string(i) + ".in");
		cout << i << " ";
	}
	cout << endl;

	// 虚拟机测试
	int numVMCase = 30;
	for (int i = 1; i <= numVMCase; ++i)
	{
		string path = "src/ByxParser/Test/TestCase/VMTest/";
		CheckVMCase(path + to_string(i) + ".in", path + to_string(i) + ".out");
		cout << i << " ";
	}
	cout << endl;

	watch.end();
	cout << "ByxParser test passed! time: " << watch.duration() << "s" << endl;
}

void ByxParserTest::CheckCase(const string& in, const string& out)
{
	string program = FileToString(in);
	ifstream fin(out);
	if (!fin)
	{
		cout << "Open file failed: " << out << endl;
		exit(0);
	}

	ByxParser parser(program);

	try
	{
		parser.parse();
	}
	catch (ByxParser::ParseError err)
	{
		cout << "Parser test failed at file: " << in << endl;
		cout << err.getMsg() << endl;
		cout << "row: " << err.getRow() << endl;
		cout << "col: " << err.getCol() << endl;
		exit(0);
	}

	FunctionTable table = parser.getFunctionTable();
	Code code = parser.getCode();

	//cout << code.toString() << endl;
	//cout << table.toString() << endl;

	// 验证函数表

	int numFunctions;
	fin >> numFunctions;
	if (numFunctions != table.getCount())
	{
		cout << "Parser test failed at file " << in << endl;
		cout << numFunctions << " " << table.getCount() << endl;
		exit(0);
	}

	for (int i = 0; i < numFunctions; ++i)
	{
		int space, addr;
		fin >> space >> addr;
		if (!(table.getSpace(i) == space && table.getAddr(i) == addr))
		{
			cout << "Parser test failed at file " << in << endl;
			cout << "Function info inconsistent." << endl;
			cout << space << " " << addr << endl;
			cout << table.getSpace(i) << " " << table.getAddr(i) << endl;
			exit(0);
		}
	}

	// 验证全局变量空间
	int globalSpace;
	fin >> globalSpace;
	if (globalSpace != parser.getGlobalSpace())
	{
		cout << "Parser test failed at file " << in << endl;
		cout << "Global space inconsistent." << endl;
		exit(0);
	}

	// 验证生成代码
	vector<Instruction> insts = code.getInsts();
	string str;
	for (int i = 0; i < (int)insts.size(); ++i)
	{
		str += insts[i].toString();
		str += " ";
	}

	stringstream ss(str);
	string s;
	while (ss >> s)
	{
		string s1;
		fin >> s1;
		if (s != s1)
		{
			cout << "Parser test failed at file " << in << endl;
			cout << s << " " << s1 << endl;
			exit(0);
		}
	}
}

void ByxParserTest::CheckErrorCase(const std::string& in)
{
	string program = FileToString(in);
	try
	{
		ByxParser(program).parse();
	}
	catch (...)
	{
		return;
	}

	cout << "Parser error test failed at file: " << in << endl;
	exit(0);
}

void ByxParserTest::CheckVMCase(const std::string& in, const std::string& out)
{
	string program = FileToString(in);

	try
	{
		ByxParser parser(program);
		parser.parse();
		//cout << parser.getCode().toString() << endl;
		ByxVM vm(parser.getCode(), parser.getFunctionTable(), parser.getGlobalSpace());
		vm.exec();
		vector<Value> globalVars = vm.getGlobalVars();

		/*for (int i = 0; i < (int)globalVars.size(); ++i)
		{
			cout << globalVars[i].toString() << endl;
		}
		cout << endl;*/

		ifstream fin(out);
		if (!fin)
		{
			cout << "Failed to open file: " << out << endl;
			exit(0);
		}

		int i = 0;
		string type;
		while (fin >> type)
		{
			if (type == "i")
			{
				int val;
				fin >> val;
				if (val != globalVars[i].getIntVal())
				{
					cout << "Parser Test failed at file: " << in << endl;
					cout << val << " " << globalVars[i].getIntVal() << endl;
					exit(0);
				}
			}
			else if (type == "d")
			{
				double val;
				fin >> val;
				if (fabs(val - globalVars[i].getDoubleVal()) > 1e-6)
				{
					cout << "Parser Test failed at file: " << in << endl;
					cout << val << " " << globalVars[i].getDoubleVal() << endl;
					exit(0);
				}
			}
			else
			{
				cout << "Test data format incurrect." << endl;
				exit(0);
			}

			i++;
		}
	}
	catch (ByxParser::ParseError err)
	{
		cout << "VM test failed at file: " << in << endl;
		cout << err.getMsg() << endl;
		exit(0);
	}
	catch (ByxVM::ByxVMError err)
	{
		cout << "VM test failed at file: " << in << endl;
		cout << err.msg << endl;
		exit(0);
	}
	catch (...)
	{
		cout << "VM test failed at file: " << in << endl;
		cout << "Unknown error" << endl;
		exit(0);
	}
}
