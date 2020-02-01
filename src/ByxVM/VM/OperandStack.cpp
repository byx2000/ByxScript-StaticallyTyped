#include "OperandStack.h"

#include <vector>

using namespace std;

void OperandStack::pushInt(int val)
{
	operands.push(Value(val));
}

void OperandStack::pushDouble(double val)
{
	operands.push(Value(val));
}

int OperandStack::popInt()
{
	checkStack();
	int val = operands.top().getIntVal();
	operands.pop();
	return val;
}

double OperandStack::popDouble()
{
	checkStack();
	double val = operands.top().getDoubleVal();
	operands.pop();
	return val;
}

Value OperandStack::pop()
{
	Value val = operands.top();
	operands.pop();
	return val;
}

bool OperandStack::empty() const
{
	return operands.empty();
}

std::string OperandStack::toString() const
{
	string s = "operand stack:\n";
	s += "Type\tValue\n";
	
	stack<Value> t = operands;
	while (!t.empty())
	{
		s += t.top().toString();
		s += "\n";
		t.pop();
	}

	return s;
}

void OperandStack::checkStack() const
{
	if (operands.empty())
	{
		throw PopWhenStackEmpty();
	}
}
