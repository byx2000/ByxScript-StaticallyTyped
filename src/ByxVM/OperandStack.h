#pragma once

#include "Value.h"

#include <stack>

class OperandStack
{
public:
	void pushInt(int val); // 压入整数
	void pushDouble(double val); // 压入浮点数
	int popInt(); // 弹出整数
	double popDouble(); // 弹出浮点数
	Value pop(); // 弹出数值
	bool empty() const; // 是否为空

	std::string toString() const;

	// 异常
	class PopWhenStackEmpty {}; // pop时栈为空

private:
	std::stack<Value> operands;
	void checkStack() const;
};