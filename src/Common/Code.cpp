#include "Code.h"

#include <sstream>

using namespace std;



Code::Code(const std::vector<Instruction>& insts)
	: insts(insts), PC(0)
{
	
}

Code::Code(const CodeSeg& seg)
{
	insts = seg.getInsts();
	PC = 0;
}

void Code::add(Opcode op)
{
	insts.push_back(Instruction(op));
}

void Code::add(Opcode op, int p)
{
	insts.push_back(Instruction(op, p));
}

void Code::add(Opcode op, double p)
{
	insts.push_back(Instruction(op, p));
}

void Code::add(const Instruction& inst)
{
	insts.push_back(inst);
}

/*void Code::add(const std::vector<Instruction>& v)
{
	for (int i = 0; i < (int)v.size(); ++i)
	{
		insts.push_back(v[i]);
	}
}*/

Instruction& Code::getNextInst()
{
	static Instruction last(Opcode::halt);

	// 到达指令流末尾，返回halt指令
	if (PC == insts.size())
	{
		return last;
	}

	return insts[PC++];
}

int Code::getPC() const
{
	return PC;
}

void Code::setPC(int addr)
{
	if (addr < 0 || addr >= (int)insts.size())
	{
		throw IllegalPCValue();
	}
	PC = addr;
}

int Code::getCount() const
{
	return insts.size();
}

std::vector<Instruction> Code::getInsts() const
{
	return insts;
}

void Code::clear()
{
	insts.clear();
	PC = 0;
}

Instruction Code::getLastInst() const
{
	if (insts.size() > 0)
	{
		return insts.back();
	}
	else
	{
		return Instruction(Opcode::halt);
	}
}

std::string Code::toString() const
{
	string s = "code:\n";
	for (int i = 0; i < (int)insts.size(); ++i)
	{
		s += to_string(i);
		s += "\t";
		s += insts[i].toString();
		s += "\n";
	}
	return s;
}
