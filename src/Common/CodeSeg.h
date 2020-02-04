#pragma once

#include "../Common/Instruction.h"

#include <vector>
#include <map>

class CodeSeg
{
public:
	int add(Opcode op);
	int add(Opcode op, int p);
	int add(Opcode op, double p);
	int add(const Instruction& inst);
	void add(const CodeSeg& seg);
	void setIntParam(int index, int p);
	void addJumpLabel(Opcode op, const std::string& label);
	void setJumpLabel(const std::string& label, int addr);
	std::vector<Instruction> getInsts() const;
	int getSize() const;
	std::string toString() const;

private:
	std::vector<Instruction> insts;
	std::map<std::string, int> labelIndex;
};