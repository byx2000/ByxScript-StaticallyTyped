#pragma once

#include "../../Common/DataType.h"

struct FunctionInfo
{
	int index; // �ں������е�����
	int space; // �ֲ������ռ��С
	int paramCount; // ��������
	DataType retType; // ����ֵ����

	FunctionInfo(); //����map
	FunctionInfo(int index, int space, int paramCount, DataType retType);
};