#pragma once

#include <string>

enum class Opcode
{
	// �������ָ��
	iconst,
	iload,
	istore,
	iadd,
	isub,
	imul,
	idiv,
	rem, // ȡ��
	icmp,
	ig,
	ige,
	il,
	ile,
	ie,
	ine,
	iand,
	ior,
	inot,
	igstore,
	igload,
	ineg,

	// ���������ָ��
	dconst,
	dload,
	dstore,
	dadd,
	dsub,
	dmul,
	ddiv,
	dcmp,
	dg,
	dge,
	dl,
	dle,
	de,
	dne,
	dgstore,
	dgload,
	dneg,

	// ����ָ��
	nop,
	halt,
	inc,
	jmp,
	jl,
	jle,
	jg,
	jge,
	je,
	jne,
	call,
	ret,
	toi, // ǿ��ת��������
	tod, // ǿ��ת���ɸ�����
	pop, // ����ջ��������
	land, // �߼���
	lor, // �߼���
	lnot, // �߼���
};

std::string GetOpcodeSymbol(Opcode op);