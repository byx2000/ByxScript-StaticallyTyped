#include <string>

class ByxParserTest
{
public:
	static void Run();
	
private:
	static void CheckErrorCase(const std::string& in);
	static void CheckVMCase(const std::string& in, const std::string& out);
};