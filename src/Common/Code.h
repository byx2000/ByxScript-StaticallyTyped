#pragma once

#include "Instruction.h"

#include <vector>

class Code
{
public:
	Code(const std::vector<Instruction>& insts = std::vector<Instruction>());
	void add(Opcode op); // ����޲���ָ��
	void add(Opcode op, int p); // �����������ָ��
	void add(Opcode op, double p); // ��Ӹ���������ָ��
	void add(const Instruction& inst); // ���ָ��
	void add(const std::vector<Instruction>& v); // ��Ӷ���ָ��
	Instruction& getNextInst(); // ��ȡ��һ��ָ��
	int getPC() const; // ��ȡ���������
	void setPC(int addr); // ���ó��������
	int getCount() const; // ��ȡָ������
	std::vector<Instruction> getInsts() const; // ��ȡ����ָ��
	void clear(); // ���
	Instruction getLastInst() const; // ��ȡ���һ��ָ��

	std::string toString() const;

	// �쳣
	class IllegalPCValue {};

private:
	std::vector<Instruction> insts;
	int PC;
};