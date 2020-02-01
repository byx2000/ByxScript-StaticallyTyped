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

	// �쳣
	class ReadUnknownValueType {}; // ��ȡδ֪����ֵ

private:
	ValueType type;

	union
	{
		int iVal;
		double dVal;
	};
};