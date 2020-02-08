#include "ByxVM.h"

#include <iostream>

using namespace std;

ByxVM::ByxVM(const Code& code, const FunctionTable& functionTable, int globalSpace)
	: code(code), functionTable(functionTable)
{
	globalVars.resize(globalSpace);
}

void ByxVM::exec()
{
	// 为main函数创建栈帧，返回地址为-1
	callStack.push(functionTable.getAllInfo(0), -1);

	// 跳转到main函数
	code.setPC(functionTable.getAddr(0));

	try // 捕获所有组件的异常
	{

	// 按顺序执行指令
	int cntFunction = functionTable.getCount();
	Instruction& inst = code.next();
	Opcode op = inst.getOp();
	while (op != Opcode::halt)
	{
		switch (op)
		{

		/// 整数相关指令 ///

		case Opcode::iconst: // 将整形常数压入操作数栈
		{
			int val = inst.getIntParam();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::iload: // 将整形变量压入操作数栈
		{
			int index = inst.getIntParam();
			int val = callStack.top().getIntVar(index);
			operandStack.pushInt(val);
			break;
		}
		case Opcode::istore: // 将栈顶的整形值存入变量
		{
			int index = inst.getIntParam();
			int val = operandStack.popInt();
			callStack.top().setIntVar(index, val);
			break;
		}
		case Opcode::iadd: // 整数加法
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a + b);
			break;
		}
		case Opcode::isub: // 整数减法
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a - b);
			break;
		}
		case Opcode::imul: // 整数乘法
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a * b);
			break;
		}
		case Opcode::idiv: // 整数除法
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			if (b == 0)
			{
				throw ByxVMError("Divided by zero.");
			}
			operandStack.pushInt(a / b);
			break;
		}
		case Opcode::rem: // 取余
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			if (b == 0)
			{
				throw ByxVMError("Remainder cannot be zero.");
			}
			operandStack.pushInt(a % b);
			break;
		}
		case Opcode::icmp: // 整数比较
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			if (a < b)
			{
				operandStack.pushInt(-1);
			}
			else if (a == b)
			{
				operandStack.pushInt(0);
			}
			else
			{
				operandStack.pushInt(1);
			}
			break;
		}
		case Opcode::ig: // 大于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a > b);
			break;
		}
		case Opcode::ige: // 大于等于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a >= b);
			break;
		}
		case Opcode::il: // 小于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a < b);
			break;
		}
		case Opcode::ile: // 小于等于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a <= b);
			break;
		}
		case Opcode::ie: // 等于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a == b);
			break;
		}
		case Opcode::ine: // 不等于
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a != b);
			break;
		}
		case Opcode::iand: // 与
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a & b);
			break;
		}
		case Opcode::ior: // 或
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a | b);
			break;
		}
		case Opcode::inot: // 非
		{
			int v = operandStack.popInt();
			operandStack.pushInt(~v);
			break;
		}
		case Opcode::igstore: // 整数全局变量存储
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // 下标合法性检查
			{
				throw ByxVMError("Global var index out of range.");
			}
			int val = operandStack.popInt();
			globalVars[index] = Value(val);
			break;
		}
		case Opcode::igload: // 整数全局变量读取
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // 下标合法性检查
			{
				throw ByxVMError("Global var index out of range.");
			}
			int val = globalVars[index].getIntVal();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::ineg: // 取相反数
		{
			int val = operandStack.popInt();
			operandStack.pushInt(-val);
			break;
		}

		/// 浮点数相关指令 ///

		case Opcode::dconst: // 将浮点型常数压栈
		{
			double val = inst.getDoubleParam();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dload: // 将浮点型变量压栈
		{
			int index = inst.getIntParam();
			double val = callStack.top().getDoubleVar(index);
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dstore: // 将栈顶的浮点型值存入变量
		{
			int index = inst.getIntParam();
			double val = operandStack.popDouble();
			callStack.top().setDoubleVar(index, val);
			break;
		}
		case Opcode::dadd: // 浮点数加法
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a + b);
			break;
		}
		case Opcode::dsub: // 浮点数减法
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a - b);
			break;
		}
		case Opcode::dmul: // 浮点数乘法
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a * b);
			break;
		}
		case Opcode::ddiv: // 浮点数除法
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			if (fabs(b) < 1e-6)
			{
				throw ByxVMError("Divided by zero.");
			}
			operandStack.pushDouble(a / b);
			break;
		}
		case Opcode::dcmp: // 浮点数比较
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			if (a < b)
			{
				operandStack.pushInt(-1);
			}
			else if (a == b)
			{
				operandStack.pushInt(0);
			}
			else
			{
				operandStack.pushInt(1);
			}
			break;
		}
		case Opcode::dg: // 大于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a > b);
			break;
		}
		case Opcode::dge: // 大于等于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a >= b);
			break;
		}
		case Opcode::dl: // 小于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a < b);
			break;
		}
		case Opcode::dle: // 小于等于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a <= b);
			break;
		}
		case Opcode::de: // 等于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a == b);
			break;
		}
		case Opcode::dne: // 不等于
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a != b);
			break;
		}
		case Opcode::dgstore: // 浮点数全局变量存储
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // 下标合法性检查
			{
				throw ByxVMError("Global var index out of range.");
			}
			double val = operandStack.popDouble();
			globalVars[index] = Value(val);
			break;
		}
		case Opcode::dgload: // 浮点数全局变量读取
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // 下标合法性检查
			{
				throw ByxVMError("Global var index out of range.");
			}
			double val = globalVars[index].getDoubleVal();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dneg: // 取相反数
		{
			double val = operandStack.popDouble();
			operandStack.pushDouble(-val);
			break;
		}

		/// 其他指令 ///

		case Opcode::nop: // 什么都不做
		{
			break;
		}	
		case Opcode::halt: // 停止执行
		{
			return;
		}
		case Opcode::inc: // 将栈顶整数值增加1
		{
			int val = operandStack.popInt();
			val++;
			operandStack.pushInt(val);
			break;
		}
		case Opcode::jmp: // 无条件跳转
		{
			int addr = inst.getIntParam();
			code.setPC(addr);
			break;
		}
		case Opcode::jl: // 小于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.lessThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jle: // 小于等于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.lessEqualThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jg: // 大于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.greaterThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jge: // 大于等于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.greaterEqualThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::je: // 等于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.isZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jne: // 不等于0时跳转
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (!val.isZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::call: // 函数调用
		{
			int funIndex = inst.getIntParam();
			const FunctionTable::Entry& info = functionTable.getAllInfo(funIndex);
			int retAddr = code.getPC(); callStack.push(info, retAddr);
			code.setPC(info.addr);

			break;
		}
		case Opcode::ret: // 函数返回
		{
			int retAddr = callStack.pop();
			if (retAddr == -1) // main函数返回
			{
				return;
			}
			code.setPC(retAddr);
			break;
		}
		case Opcode::toi: // 将栈顶元素强制转换成整数
		{
			int val = operandStack.popInt();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::tod: // 将栈顶元素强制转换成浮点数
		{
			double val = operandStack.popDouble();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::pop: // 弹出栈顶操作数
		{
			operandStack.pop();
			break;
		}
		case Opcode::land: // 逻辑与
		{
			Value b = operandStack.popInt();
			Value a = operandStack.popInt();
			operandStack.pushInt(!a.isZero() && !b.isZero());
			break;
		}
		case Opcode::lor: // 逻辑或
		{
			Value b = operandStack.popInt();
			Value a = operandStack.popInt();
			operandStack.pushInt(!a.isZero() || !b.isZero());
			break;
		}
		case Opcode::lnot: // 逻辑非
		{
			Value v = operandStack.pop();
			operandStack.pushInt(v.isZero());
			break;
		}
		default: // 未识别的指令
			throw ByxVMError("Illegal instruction: " + GetOpcodeSymbol(op) + ".");
			break;
		}

		inst = code.next();
		op = inst.getOp();
	}

	} 
	// 捕获Value异常
	catch (Value::ReadUnknownDataType)
	{
		throw ByxVMError("Read unknown value type.");
	}
	// 捕获OperandStack异常
	catch (OperandStack::PopWhenStackEmpty)
	{
		throw ByxVMError("Operand stack is empty when pop.");
	}
	// 捕获CallStack异常
	catch (CallStack::PopWhenStackEmpty)
	{
		throw ByxVMError("Call stack is empty when pop.");
	}
	catch (CallStack::VarIndexOutOfBoundary)
	{
		throw ByxVMError("Illegal var index.");
	}
	// 捕获Code异常
	catch (Code::IllegalPCValue)
	{
		throw ByxVMError("Illegal PC value to set.");
	}
	// 捕获FunctionTable异常
	catch (FunctionTable::FunctionIndexOutOfBoundary)
	{
		throw ByxVMError("Illegal function index.");
	}
	// 捕获ByxVM异常
	catch (ByxVMError err)
	{
		throw err;
	}
	// 未知异常
	catch (...)
	{
		throw ByxVMError("Unknown error.");
	}
}

OperandStack ByxVM::getOpStack() const
{
	return operandStack;
}

std::vector<Value> ByxVM::getGlobalVars() const
{
	return globalVars;
}

ByxVM::ByxVMError::ByxVMError(const std::string& msg)
	: msg(msg)
{

}
