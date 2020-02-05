#pragma once

#include "DataType.h"

class Value
{
public:
	Value();
	Value(int val);
	Value(double val);
	DataType getType() const;
	int getIntVal() const;
	double getDoubleVal() const;
	bool isZero() const;
	std::string toString() const;

	// 异常
	class ReadUnknownDataType {}; // 读取未知类型值

private:
	DataType type;

	union
	{
		int iVal;
		double dVal;
	};
};