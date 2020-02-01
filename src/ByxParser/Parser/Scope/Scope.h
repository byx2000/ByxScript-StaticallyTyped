#pragma once

#include "../../../Common/DataType.h"
#include "../SymbolInfo.h"

#include <map>

class Scope
{
public:
	void define(const std::string& name, SymbolInfo symbol); // �������
	SymbolInfo resolve(const std::string& name); // ��������
	int getSymbolCount() const; // ��ȡ��������

	// �쳣
	class SymbolNotFound {}; // ����δ����
	class SymbolRedefined {}; // �����ض���

private:
	std::map<std::string, SymbolInfo> symbolTable;
};