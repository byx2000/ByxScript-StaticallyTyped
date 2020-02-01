#pragma once

#include "Value.h"

#include <stack>

class OperandStack
{
public:
	void pushInt(int val); // ѹ������
	void pushDouble(double val); // ѹ�븡����
	int popInt(); // ��������
	double popDouble(); // ����������
	Value pop(); // ������ֵ
	bool empty() const; // �Ƿ�Ϊ��

	std::string toString() const;

	// �쳣
	class PopWhenStackEmpty {}; // popʱջΪ��

private:
	std::stack<Value> operands;
	void checkStack() const;
};