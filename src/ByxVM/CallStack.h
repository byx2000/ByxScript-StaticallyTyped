#pragma once

#include "../Common/FunctionTable.h"
#include "../Common/Value.h"

#include <stack>

class CallStack
{
public:
	// ����ջ��ջ֡
	class Frame
	{
	public:
		Frame(const FunctionTable::Entry& info, int retAddr);
		int getIntVar(int index) const; // ��ȡ��������
		double getDoubleVar(int index) const; // ��ȡ����������
		void setIntVar(int index, int val); // ������������
		void setDoubleVar(int index, double val); // ���ø���������
		int getReturnAddr() const; // ��ȡ���ص�ַ
		int getVarSpace() const; // ��ȡ�����ռ�

		std::string toString() const;

	private:
		const FunctionTable::Entry& info; // ������Ϣ
		std::vector<Value> varTable; // ������
		int retAddr; // ���ص�ַ

		void checkIndex(int index) const; // �±���
	};

	void push(const FunctionTable::Entry& info, int retAddr); // ѹ��ջ֡
	int pop(); // ����ջ֡�����ط��ص�ַ
	Frame& top(); // ��ȡ����ջջ��

	// �쳣��
	class PopWhenStackEmpty {}; // popʱ����ջΪ��
	class VarIndexOutOfBoundary {}; // ��������Խ��

private:
	std::stack<Frame> calls; // ����ջ
};