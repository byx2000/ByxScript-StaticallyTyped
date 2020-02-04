#include "Opcode.h"

using namespace std;

static string Symbols[] =
{
	// �������ָ��
	"iconst",
	"iload",
	"istore",
	"iadd",
	"isub",
	"imul",
	"idiv",
	"rem",
	"icmp",
	"ig",
	"ige",
	"il",
	"ile",
	"ie",
	"ine",
	"iand",
	"ior",
	"inot",
	"igstore",
	"igload",
	
	// ���������ָ��
	"dconst",
	"dload",
	"dstore",
	"dadd",
	"dsub",
	"dmul",
	"ddiv",
	"dcmp",
	"dg",
	"dge",
	"dl",
	"dle",
	"de",
	"dne",
	"dgstore",
	"dgload",

	// ����ָ��
	"nop",
	"halt",
	"inc",
	"jmp",
	"jl",
	"jle",
	"jg",
	"jge",
	"je",
	"jne",
	"call",
	"ret",
	"toi", // ǿ��ת��������
	"tod", // ǿ��ת���ɸ�����
	"pop", // ����ջ��������
};

std::string GetOpcodeSymbol(Opcode op)
{
	return Symbols[(int)op];
}
