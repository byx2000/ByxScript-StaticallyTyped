#include "Value.h"

using namespace std;

Value::Value()
	: iVal(0), type(DataType::Unknown)
{

}

Value::Value(int val)
	: iVal(val), type(DataType::Integer)
{
	
}

Value::Value(double val)
	: dVal(val), type(DataType::Double)
{

}

DataType Value::getType() const
{
	return type;
}

int Value::getIntVal() const
{
	if (type == DataType::Integer)
	{
		return iVal;
	}
	else if (type == DataType::Double)
	{
		return (int)dVal;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

double Value::getDoubleVal() const
{
	if (type == DataType::Integer)
	{
		return (double)iVal;
	}
	else if (type == DataType::Double)
	{
		return dVal;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

bool Value::isZero() const
{
	if (type == DataType::Integer)
	{
		return iVal == 0;
	}
	else if (type == DataType::Double)
	{
		return dVal == 0.0;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

bool Value::lessThanZero() const
{
	if (type == DataType::Integer)
	{
		return iVal < 0;
	}
	else if (type == DataType::Double)
	{
		return dVal < 0.0;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

bool Value::lessEqualThanZero() const
{
	if (type == DataType::Integer)
	{
		return iVal <= 0;
	}
	else if (type == DataType::Double)
	{
		return dVal <= 0.0;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

bool Value::greaterEqualThanZero() const
{
	if (type == DataType::Integer)
	{
		return iVal >= 0;
	}
	else if (type == DataType::Double)
	{
		return dVal >= 0.0;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

bool Value::greaterThanZero() const
{
	if (type == DataType::Integer)
	{
		return iVal > 0;
	}
	else if (type == DataType::Double)
	{
		return dVal > 0.0;
	}
	else
	{
		throw ReadUnknownDataType();
	}
}

std::string Value::toString() const
{
	string s = "";
	if (type == DataType::Integer)
	{
		s += "int\t";
		s += to_string(iVal);
	}
	else if (type == DataType::Double)
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
