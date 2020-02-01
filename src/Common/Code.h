#pragma once

#include "Instruction.h"

#include <vector>

class Code
{
public:
	Code(const std::vector<Instruction>& insts = std::vector<Instruction>());
	void add(Opcode op); // 添加无参数指令
	void add(Opcode op, int p); // 添加整数参数指令
	void add(Opcode op, double p); // 添加浮点数参数指令
	void add(const Instruction& inst); // 添加指令
	void add(const std::vector<Instruction>& v); // 添加多条指令
	Instruction& getNextInst(); // 获取下一条指令
	int getPC() const; // 获取程序计数器
	void setPC(int addr); // 设置程序计数器
	int getCount() const; // 获取指令条数
	std::vector<Instruction> getInsts() const; // 获取所有指令
	void clear(); // 清空
	Instruction getLastInst() const; // 获取最后一条指令

	std::string toString() const;

	// 异常
	class IllegalPCValue {};

private:
	std::vector<Instruction> insts;
	int PC;
};