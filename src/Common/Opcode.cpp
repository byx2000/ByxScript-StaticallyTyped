#include "Opcode.h"

using namespace std;

static string Symbols[] =
{
	// 整数相关指令
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
	
	// 浮点数相关指令
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

	// 其他指令
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
	"toi", // 强制转换成整数
	"tod", // 强制转换成浮点数
	"pop", // 弹出栈顶操作数
};

std::string GetOpcodeSymbol(Opcode op)
{
	return Symbols[(int)op];
}
