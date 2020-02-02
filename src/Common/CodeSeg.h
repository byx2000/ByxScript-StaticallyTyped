#pragma once

#include "../Common/Instruction.h"

#include <vector>

class CodeSeg
{
public:
	void add(Opcode op);
	void add(Opcode op, int p);
	void add(Opcode op, double p);
	void add(const Instruction& inst);
	void add(CodeSeg& seg);
	std::vector<Instruction> getInsts() const;
	int getSize() const;

private:
	std::vector<Instruction> insts;
	void relocation(int base); // ÷ÿ∂®Œª
};