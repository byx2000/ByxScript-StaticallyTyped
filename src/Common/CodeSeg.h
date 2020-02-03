#pragma once

#include "../Common/Instruction.h"

#include <vector>
#include <map>

class CodeSeg
{
public:
	void add(Opcode op);
	void add(Opcode op, int p);
	void add(Opcode op, double p);
	void add(const Instruction& inst);
	void add(const CodeSeg& seg);
	void addJumpLabel(Opcode op, const std::string& label);
	void setJumpLabel(const std::string& label, int addr);
	std::vector<Instruction> getInsts() const;
	int getSize() const;
	std::string toString() const;

private:
	std::vector<Instruction> insts;
	std::map<std::string, int> labelIndex;
};