#pragma once

#include <string>

enum class ValueType
{
	Integer,
	Double,
	Unknown,
};

class Value
{
public:
	Value();
	Value(int val);
	Value(double val);
	ValueType getValueType() const;
	int getIntVal() const;
	double getDoubleVal() const;
	std::string toString() const;

	// 异常
	class ReadUnknownValueType {}; // 读取未知类型值

private:
	ValueType type;

	union
	{
		int iVal;
		double dVal;
	};
};