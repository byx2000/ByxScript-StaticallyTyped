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

	// “Ï≥£≤‚ ‘
	int numErrorCase = 37;
	for (int i = 1; i <= numErrorCase; ++i)
	{
		string path = "src/ByxParser/Test/TestCase/Error/";
		CheckErrorCase(path + to_string(i) + ".in");
		cout << i << " ";
	}
	cout << endl;

	// –Èƒ‚ª˙≤‚ ‘
	int numVMCase = 30;
	for (int i = 1; i <= numVMCase; ++i)
	{
		string path = "src/ByxParser/Test/TestCase/VM/";
		CheckVMCase(path + to_string(i) + ".in", path + to_string(i) + ".out");
		cout << i << " ";
	}
	cout << endl;

	watch.end();
	cout << "ByxParser test passed! time: " << watch.duration() << "s" << endl;
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
