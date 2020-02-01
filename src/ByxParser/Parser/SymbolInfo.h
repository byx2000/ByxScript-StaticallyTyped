#pragma once

#include <string>
#include "../../Common/DataType.h"

struct SymbolInfo
{
	DataType type; // 符号类型
	int index; // 符号在变量表中的索引
	bool isGlobal; // 是否为全局变量

	SymbolInfo(); // 用于map
	SymbolInfo(DataType type, int index, bool isGlobal);
};