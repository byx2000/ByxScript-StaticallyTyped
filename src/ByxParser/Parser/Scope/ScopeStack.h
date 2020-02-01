#pragma once

#include "Scope.h"
#include "../../../Common/DataType.h"

#include <vector>

class ScopeStack
{
public:
	ScopeStack();
	void pushScope(); // ѹ��������ջ
	void popScope(); // ����������ջ
	int define(const std::string& name, DataType type); // ������ţ��Զ�����������
	SymbolInfo resolve(const std::string& name); // ��������
	int getSpace() const; // ��ȡ����ռ�õĿռ䣨����symbolsCount��

	// �쳣
	class SymbolNotFound {}; // ����δ����
	class SymbolRedefined {}; // �����ض���
	class EmptyStack {}; // ִ�в���ʱ������ջΪ��

private:
	std::vector<Scope> scopes;
	int symbolsCount; // ��ǰ��������
};