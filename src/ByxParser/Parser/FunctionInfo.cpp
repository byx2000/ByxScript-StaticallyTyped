#include "FunctionInfo.h"

using namespace std;

FunctionInfo::FunctionInfo()
{
	index = 0;
	space = 0;
	paramCount = 0;
	retType = DataType::Unknown;
}

FunctionInfo::FunctionInfo(int index, int space, int paramCount, DataType retType)
	: index(index), space(space), paramCount(paramCount), retType(retType)
{

}