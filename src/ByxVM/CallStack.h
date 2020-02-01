#pragma once

#include "../Common/FunctionTable.h"
#include "Value.h"

#include <stack>

class StackFrame
{
public:
	StackFrame(const FunctionEntry& info, int retAddr);
	int getIntVar(int index) const; // ��ȡ��������
	double getDoubleVar(int index) const; // ��ȡ����������
	void setIntVar(int index, int val); // ������������
	void setDoubleVar(int index, double val); // ���ø���������
	int getReturnAddr() const; // ��ȡ���ص�ַ
	int getVarSpace() const; // ��ȡ�����ռ�

	std::string toString() const;

private:
	const FunctionEntry& info; // ������Ϣ
	std::vector<Value> varTable; // ������
	int retAddr; // ���ص�ַ

	void checkIndex(int index) const; // �±���
};

class CallStack
{
public:
	void push(const StackFrame& stackFrame); // ѹ��ջ֡
	int pop(); // ����ջ֡�����ط��ص�ַ
	StackFrame& top(); // ��ȡ����ջջ��

	// �쳣��
	class PopWhenStackEmpty {}; // popʱ����ջΪ��
	class VarIndexOutOfBoundary {}; // ��������Խ��

private:
	std::stack<StackFrame> calls; // ����ջ
};