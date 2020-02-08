#include "CallStack.h"

using namespace std;

StackFrame::StackFrame(const FunctionTable::Entry& info, int retAddr)
	: info(info), retAddr(retAddr)
{
	// 设置变量表的大小
	varTable.resize(info.space);
}

int StackFrame::getIntVar(int index) const
{
	checkIndex(index);
	return varTable[index].getIntVal();
}

double StackFrame::getDoubleVar(int index) const
{
	checkIndex(index);
	return varTable[index].getDoubleVal();
}

void StackFrame::setIntVar(int index, int val)
{
	checkIndex(index);
	varTable[index] = Value(val);
}

void StackFrame::setDoubleVar(int index, double val)
{
	checkIndex(index);
	varTable[index] = Value(val);
}

int StackFrame::getReturnAddr() const
{
	return retAddr;
}

int StackFrame::getVarSpace() const
{
	return varTable.size();
}

std::string StackFrame::toString() const
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

void StackFrame::checkIndex(int index) const
{
	if (index < 0 || index >= (int)varTable.size())
	{
		throw CallStack::VarIndexOutOfBoundary();
	}
}

void CallStack::push(const StackFrame& stackFrame)
{
	calls.push(stackFrame);
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

StackFrame& CallStack::top()
{
	return calls.top();
}
