#pragma once

#include "../../Common/DataType.h"

struct FunctionInfo
{
	int index; // 在函数表中的索引
	int space; // 局部变量空间大小
	int paramCount; // 参数个数
	DataType retType; // 返回值类型

	FunctionInfo(); //用于map
	FunctionInfo(int index, int space, int paramCount, DataType retType);
};