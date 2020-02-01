#include "FunctionTable.h"

using namespace std;

FunctionEntry::FunctionEntry(int space, int addr)
	: space(space), addr(addr)
{

}

std::string FunctionEntry::toString() const
{
	string s;
	s += to_string(space);
	s += "\t";
	s += to_string(addr);
	return s;
}

FunctionTable::FunctionTable(const std::vector<FunctionEntry>& table)
	: table(table)
{

}

void FunctionTable::add(int space, int addr)
{
	table.push_back(FunctionEntry(space, addr));
}

int FunctionTable::getSpace(int index) const
{
	checkIndex(index);
	return table[index].space;
}

int FunctionTable::getAddr(int index) const
{
	checkIndex(index);
	return table[index].addr;
}

const FunctionEntry& FunctionTable::getAllInfo(int index) const
{
	checkIndex(index);
	return table[index];
}

int FunctionTable::getCount() const
{
	return table.size();
}

void FunctionTable::setAddr(int index, int addr)
{
	checkIndex(index);
	table[index].addr = addr;
}

void FunctionTable::setSpace(int index, int space)
{
	checkIndex(index);
	table[index].space = space;
}

std::string FunctionTable::toString() const
{
	string s = "function table:\nindex\tSpace\tAddress\n";
	for (int i = 0; i < (int)table.size(); ++i)
	{
		s += to_string(i);
		if (i == 0)
		{
			s += "(main)";
		}
		s += "\t";
		s += table[i].toString();
		
		s += "\n";
	}
	return s;
}

void FunctionTable::checkIndex(int index) const
{
	if (index < 0 || index >= (int)table.size())
	{
		throw FunctionIndexOutOfBoundary();
	}
}
