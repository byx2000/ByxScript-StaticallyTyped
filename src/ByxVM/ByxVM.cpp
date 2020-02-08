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
	// Ϊmain��������ջ֡�����ص�ַΪ-1
	callStack.push(functionTable.getAllInfo(0), -1);

	// ��ת��main����
	code.setPC(functionTable.getAddr(0));

	try // ��������������쳣
	{

	// ��˳��ִ��ָ��
	int cntFunction = functionTable.getCount();
	Instruction& inst = code.next();
	Opcode op = inst.getOp();
	while (op != Opcode::halt)
	{
		switch (op)
		{

		/// �������ָ�� ///

		case Opcode::iconst: // �����γ���ѹ�������ջ
		{
			int val = inst.getIntParam();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::iload: // �����α���ѹ�������ջ
		{
			int index = inst.getIntParam();
			int val = callStack.top().getIntVar(index);
			operandStack.pushInt(val);
			break;
		}
		case Opcode::istore: // ��ջ��������ֵ�������
		{
			int index = inst.getIntParam();
			int val = operandStack.popInt();
			callStack.top().setIntVar(index, val);
			break;
		}
		case Opcode::iadd: // �����ӷ�
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a + b);
			break;
		}
		case Opcode::isub: // ��������
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a - b);
			break;
		}
		case Opcode::imul: // �����˷�
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a * b);
			break;
		}
		case Opcode::idiv: // ��������
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
		case Opcode::rem: // ȡ��
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
		case Opcode::icmp: // �����Ƚ�
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
		case Opcode::ig: // ����
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a > b);
			break;
		}
		case Opcode::ige: // ���ڵ���
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a >= b);
			break;
		}
		case Opcode::il: // С��
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a < b);
			break;
		}
		case Opcode::ile: // С�ڵ���
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a <= b);
			break;
		}
		case Opcode::ie: // ����
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a == b);
			break;
		}
		case Opcode::ine: // ������
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a != b);
			break;
		}
		case Opcode::iand: // ��
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a & b);
			break;
		}
		case Opcode::ior: // ��
		{
			int b = operandStack.popInt();
			int a = operandStack.popInt();
			operandStack.pushInt(a | b);
			break;
		}
		case Opcode::inot: // ��
		{
			int v = operandStack.popInt();
			operandStack.pushInt(~v);
			break;
		}
		case Opcode::igstore: // ����ȫ�ֱ����洢
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // �±�Ϸ��Լ��
			{
				throw ByxVMError("Global var index out of range.");
			}
			int val = operandStack.popInt();
			globalVars[index] = Value(val);
			break;
		}
		case Opcode::igload: // ����ȫ�ֱ�����ȡ
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // �±�Ϸ��Լ��
			{
				throw ByxVMError("Global var index out of range.");
			}
			int val = globalVars[index].getIntVal();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::ineg: // ȡ�෴��
		{
			int val = operandStack.popInt();
			operandStack.pushInt(-val);
			break;
		}

		/// ���������ָ�� ///

		case Opcode::dconst: // �������ͳ���ѹջ
		{
			double val = inst.getDoubleParam();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dload: // �������ͱ���ѹջ
		{
			int index = inst.getIntParam();
			double val = callStack.top().getDoubleVar(index);
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dstore: // ��ջ���ĸ�����ֵ�������
		{
			int index = inst.getIntParam();
			double val = operandStack.popDouble();
			callStack.top().setDoubleVar(index, val);
			break;
		}
		case Opcode::dadd: // �������ӷ�
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a + b);
			break;
		}
		case Opcode::dsub: // ����������
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a - b);
			break;
		}
		case Opcode::dmul: // �������˷�
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushDouble(a * b);
			break;
		}
		case Opcode::ddiv: // ����������
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
		case Opcode::dcmp: // �������Ƚ�
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
		case Opcode::dg: // ����
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a > b);
			break;
		}
		case Opcode::dge: // ���ڵ���
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a >= b);
			break;
		}
		case Opcode::dl: // С��
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a < b);
			break;
		}
		case Opcode::dle: // С�ڵ���
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a <= b);
			break;
		}
		case Opcode::de: // ����
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a == b);
			break;
		}
		case Opcode::dne: // ������
		{
			double b = operandStack.popDouble();
			double a = operandStack.popDouble();
			operandStack.pushInt(a != b);
			break;
		}
		case Opcode::dgstore: // ������ȫ�ֱ����洢
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // �±�Ϸ��Լ��
			{
				throw ByxVMError("Global var index out of range.");
			}
			double val = operandStack.popDouble();
			globalVars[index] = Value(val);
			break;
		}
		case Opcode::dgload: // ������ȫ�ֱ�����ȡ
		{
			int index = inst.getIntParam();
			if (index < 0 || index >= (int)globalVars.size()) // �±�Ϸ��Լ��
			{
				throw ByxVMError("Global var index out of range.");
			}
			double val = globalVars[index].getDoubleVal();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::dneg: // ȡ�෴��
		{
			double val = operandStack.popDouble();
			operandStack.pushDouble(-val);
			break;
		}

		/// ����ָ�� ///

		case Opcode::nop: // ʲô������
		{
			break;
		}	
		case Opcode::halt: // ִֹͣ��
		{
			return;
		}
		case Opcode::inc: // ��ջ������ֵ����1
		{
			int val = operandStack.popInt();
			val++;
			operandStack.pushInt(val);
			break;
		}
		case Opcode::jmp: // ��������ת
		{
			int addr = inst.getIntParam();
			code.setPC(addr);
			break;
		}
		case Opcode::jl: // С��0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.lessThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jle: // С�ڵ���0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.lessEqualThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jg: // ����0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.greaterThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jge: // ���ڵ���0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.greaterEqualThanZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::je: // ����0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (val.isZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::jne: // ������0ʱ��ת
		{
			int addr = inst.getIntParam();
			Value val = operandStack.pop();
			if (!val.isZero())
			{
				code.setPC(addr);
			}
			break;
		}
		case Opcode::call: // ��������
		{
			int funIndex = inst.getIntParam();
			const FunctionTable::Entry& info = functionTable.getAllInfo(funIndex);
			int retAddr = code.getPC(); callStack.push(info, retAddr);
			code.setPC(info.addr);

			break;
		}
		case Opcode::ret: // ��������
		{
			int retAddr = callStack.pop();
			if (retAddr == -1) // main��������
			{
				return;
			}
			code.setPC(retAddr);
			break;
		}
		case Opcode::toi: // ��ջ��Ԫ��ǿ��ת��������
		{
			int val = operandStack.popInt();
			operandStack.pushInt(val);
			break;
		}
		case Opcode::tod: // ��ջ��Ԫ��ǿ��ת���ɸ�����
		{
			double val = operandStack.popDouble();
			operandStack.pushDouble(val);
			break;
		}
		case Opcode::pop: // ����ջ��������
		{
			operandStack.pop();
			break;
		}
		case Opcode::land: // �߼���
		{
			Value b = operandStack.popInt();
			Value a = operandStack.popInt();
			operandStack.pushInt(!a.isZero() && !b.isZero());
			break;
		}
		case Opcode::lor: // �߼���
		{
			Value b = operandStack.popInt();
			Value a = operandStack.popInt();
			operandStack.pushInt(!a.isZero() || !b.isZero());
			break;
		}
		case Opcode::lnot: // �߼���
		{
			Value v = operandStack.pop();
			operandStack.pushInt(v.isZero());
			break;
		}
		default: // δʶ���ָ��
			throw ByxVMError("Illegal instruction: " + GetOpcodeSymbol(op) + ".");
			break;
		}

		inst = code.next();
		op = inst.getOp();
	}

	} 
	// ����Value�쳣
	catch (Value::ReadUnknownDataType)
	{
		throw ByxVMError("Read unknown value type.");
	}
	// ����OperandStack�쳣
	catch (OperandStack::PopWhenStackEmpty)
	{
		throw ByxVMError("Operand stack is empty when pop.");
	}
	// ����CallStack�쳣
	catch (CallStack::PopWhenStackEmpty)
	{
		throw ByxVMError("Call stack is empty when pop.");
	}
	catch (CallStack::VarIndexOutOfBoundary)
	{
		throw ByxVMError("Illegal var index.");
	}
	// ����Code�쳣
	catch (Code::IllegalPCValue)
	{
		throw ByxVMError("Illegal PC value to set.");
	}
	// ����FunctionTable�쳣
	catch (FunctionTable::FunctionIndexOutOfBoundary)
	{
		throw ByxVMError("Illegal function index.");
	}
	// ����ByxVM�쳣
	catch (ByxVMError err)
	{
		throw err;
	}
	// δ֪�쳣
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
