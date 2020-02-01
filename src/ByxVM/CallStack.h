#pragma once

#include "../Common/FunctionTable.h"
#include "Value.h"

#include <stack>

class StackFrame
{
public:
	StackFrame(const FunctionEntry& info, int retAddr);
	int getIntVar(int index) const; // 获取整数变量
	double getDoubleVar(int index) const; // 获取浮点数变量
	void setIntVar(int index, int val); // 设置整数变量
	void setDoubleVar(int index, double val); // 设置浮点数变量
	int getReturnAddr() const; // 获取返回地址
	int getVarSpace() const; // 获取变量空间

	std::string toString() const;

private:
	const FunctionEntry& info; // 函数信息
	std::vector<Value> varTable; // 变量表
	int retAddr; // 返回地址

	void checkIndex(int index) const; // 下标检查
};

class CallStack
{
public:
	void push(const StackFrame& stackFrame); // 压入栈帧
	int pop(); // 弹出栈帧，返回返回地址
	StackFrame& top(); // 获取调用栈栈顶

	// 异常类
	class PopWhenStackEmpty {}; // pop时调用栈为空
	class VarIndexOutOfBoundary {}; // 变量索引越界

private:
	std::stack<StackFrame> calls; // 调用栈
};