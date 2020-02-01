#pragma once

#include "Scope.h"
#include "../../../Common/DataType.h"

#include <vector>

class ScopeStack
{
public:
	ScopeStack();
	void pushScope(); // 压入作用域栈
	void popScope(); // 弹出作用域栈
	int define(const std::string& name, DataType type); // 定义符号（自动计算索引）
	SymbolInfo resolve(const std::string& name); // 解析符号
	int getSpace() const; // 获取符号占用的空间（等于symbolsCount）

	// 异常
	class SymbolNotFound {}; // 符号未定义
	class SymbolRedefined {}; // 符号重定义
	class EmptyStack {}; // 执行操作时作用域栈为空

private:
	std::vector<Scope> scopes;
	int symbolsCount; // 当前符号总数
};