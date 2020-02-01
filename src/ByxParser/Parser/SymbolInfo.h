#pragma once

#include <string>
#include "../../Common/DataType.h"

struct SymbolInfo
{
	DataType type; // ��������
	int index; // �����ڱ������е�����
	bool isGlobal; // �Ƿ�Ϊȫ�ֱ���

	SymbolInfo(); // ����map
	SymbolInfo(DataType type, int index, bool isGlobal);
};