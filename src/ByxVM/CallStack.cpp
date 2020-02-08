#include "CallStack.h"

using namespace std;

CallStack::Frame::Frame(const FunctionTable::Entry& info, int retAddr)
	: info(info), retAddr(retAddr)
{
	// ���ñ�����Ĵ�С
	varTable.resize(info.space);
}

int CallStack::Frame::getIntVar(int index) const
{
	checkIndex(index);
	return varTable[index].getIntVal();
}

double CallStack::Frame::getDoubleVar(int index) const
{
	checkIndex(index);
	return varTable[index].getDoubleVal();
}

void CallStack::Frame::setIntVar(int index, int val)
{
	checkIndex(index);
	varTable[index] = Value(val);
}

void CallStack::Frame::setDoubleVar(int index, double val)
{
	checkIndex(index);
	varTable[index] = Value(val);
}

int CallStack::Frame::getReturnAddr() const
{
	return retAddr;
}

int CallStack::Frame::getVarSpace() const
{
	return varTable.size();
}

std::string CallStack::Frame::toString() const
{
	string s = "stack frame:\n\n";
	s += "function info:\nName\tSpace\tAddress\n";
	s += info.toString();
	s += "\n\nvar table:\nIndex\tType\tValue\n";
	for (int i = 0; i < (int)varTable.size(); ++i)
	{
		s += to_string(i);
		s += "\t";
		s += varTable[i].toString();
		s += "\n";
	}
	s += "\nreturn address: ";
	s += to_string(retAddr);
	return s;
}

void CallStack::Frame::checkIndex(int index) const
{
	if (index < 0 || index >= (int)varTable.size())
	{
		throw CallStack::VarIndexOutOfBoundary();
	}
}

void CallStack::push(const FunctionTable::Entry& info, int retAddr)
{
	calls.push(Frame(info, retAddr));
}

int CallStack::pop()
{
	if (calls.empty())
	{
		throw PopWhenStackEmpty();
	}

	int retAddr = calls.top().getReturnAddr();
	calls.pop();
	return retAddr;
}

CallStack::Frame& CallStack::top()
{
	return calls.top();
}
