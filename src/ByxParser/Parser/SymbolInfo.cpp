#include "SymbolInfo.h"

using namespace std;

SymbolInfo::SymbolInfo()
{
	type = DataType::Unknown;
	index = 0;
}

SymbolInfo::SymbolInfo(DataType type, int index, bool isGlobal)
	: type(type), index(index), isGlobal(isGlobal)
{

}