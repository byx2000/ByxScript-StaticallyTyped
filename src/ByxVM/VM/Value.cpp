#include "Value.h"

using namespace std;

Value::Value()
	: iVal(0), type(ValueType::Unknown)
{

}

Value::Value(int val)
	: iVal(val), type(ValueType::Integer)
{
	
}

Value::Value(double val)
	: dVal(val), type(ValueType::Double)
{

}

ValueType Value::getValueType() const
{
	return type;
}

int Value::getIntVal() const
{
	if (type == ValueType::Integer)
	{
		return iVal;
	}
	else if (type == ValueType::Double)
	{
		return (int)dVal;
	}
	else
	{
		throw ReadUnknownValueType();
	}
}

double Value::getDoubleVal() const
{
	if (type == ValueType::Integer)
	{
		return (double)iVal;
	}
	else if (type == ValueType::Double)
	{
		return dVal;
	}
	else
	{
		throw ReadUnknownValueType();
	}
}

std::string Value::toString() const
{
	string s = "";
	if (type == ValueType::Integer)
	{
		s += "int\t";
		s += to_string(iVal);
	}
	else if (type == ValueType::Double)
	{
		s += "double\t";
		s += to_string(dVal);
	}
	else
	{
		s += "unknown";
	}
	return s;
}
