#pragma once

#include "Instruction.h"
#include "CodeSeg.h"

#include <vector>

class Code
{
public:
	Code(const CodeSeg& seg = CodeSeg());
	void add(Opcode op); // ����޲���ָ��
	void add(Opcode op, int p); // �����������ָ��
	void add(Opcode op, double p); // ��Ӹ���������ָ��
	void add(const Instruction& inst); // ���ָ��
	Instruction& getNextInst(); // ��ȡ��һ��ָ��
	int getPC() const; // ��ȡ���������
	void setPC(int addr); // ���ó��������
	int getCount() const; // ��ȡָ������
	std::vector<Instruction> getInsts() const; // ��ȡ����ָ��

	std::string toString() const;

	// �쳣
	class IllegalPCValue {};

private:
	std::vector<Instruction> insts;
	int PC;
};