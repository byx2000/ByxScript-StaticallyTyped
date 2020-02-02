#pragma once

#include "../Common/Instruction.h"

#include <vector>

class CodeSeg
{
public:
	void add(Opcode op);
	void add(Opcode op, int p);
	void add(Opcode op, double p);
	void add(CodeSeg& seg);

private:
	std::vector<Instruction> insts;
	void redirect(int base); // ÷ÿ∂®œÚ
};