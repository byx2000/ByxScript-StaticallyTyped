#pragma once

#include "../../../Common/DataType.h"
#include "../SymbolInfo.h"

#include <map>

class Scope
{
public:
	void define(const std::string& name, SymbolInfo symbol); // 定义符号
	SymbolInfo resolve(const std::string& name); // 解析符号
	int getSymbolCount() const; // 获取符号总数

	// 异常
	class SymbolNotFound {}; // 符号未定义
	class SymbolRedefined {}; // 符号重定义

private:
	std::map<std::string, SymbolInfo> symbolTable;
};