#include "CodeSeg.h"

using namespace std;

void CodeSeg::add(Opcode op)
{
	insts.push_back(Instruction(op));
}

void CodeSeg::add(Opcode op, int p)
{
	insts.push_back(Instruction(op, p));
}

void CodeSeg::add(Opcode op, double p)
{
	insts.push_back(Instruction(op, p));
}

void CodeSeg::add(const Instruction& inst)
{
	insts.push_back(inst);
}

void CodeSeg::add(CodeSeg& seg)
{
	seg.relocation(insts.size());
	for (int i = 0; i < (int)seg.insts.size(); ++i)
	{
		insts.push_back(seg.insts[i]);
	}
}

std::vector<Instruction> CodeSeg::getInsts() const
{
	return insts;
}

int CodeSeg::getSize() const
{
	return insts.size();
}

std::string CodeSeg::toString() const
{
	string s;
	for (int i = 0; i < (int)insts.size(); ++i)
	{
		s += to_string(i);
		s += "\t";
		s += insts[i].toString();
		s += "\n";
	}
	return s;
}

void CodeSeg::relocation(int base)
{
	for (int i = 0; i < (int)insts.size(); ++i)
	{
		Opcode op = insts[i].getOp();
		if (op == Opcode::jmp || op == Opcode::jl || op == Opcode::jle ||
			op == Opcode::jg || op == Opcode::jge || op == Opcode::je ||
			op == Opcode::jne)
		{
			insts[i].setIntParam(insts[i].getIntParam() + base);
		}
	}
}
