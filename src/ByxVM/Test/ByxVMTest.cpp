#include "ByxVMTest.h"
#include "../../Common/TestUtil.h"
#include "../ByxVM.h"

#include <iostream>
#include <cassert>

using namespace std;

void ByxVMTest::Run()
{
	StopWatch watch;
	watch.begin();

	// 整数存取
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::inc);
		code.add(Opcode::istore, 0);
		code.add(Opcode::istore, 1);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 301);
		ASSERT(ops.popInt() == 200);
	}

	// 浮点数存取
	{
		Code code;
		code.add(Opcode::dconst, 1.5);
		code.add(Opcode::dconst, 2.2);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 3.7);
		code.add(Opcode::dconst, 4.6);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(2, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popDouble() == 2.2);
		ASSERT(ops.popDouble() == 3.7);
		ASSERT(ops.popDouble() == 1.5);
	}

	// 整数和浮点数混合存取
	{
		Code code;
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 37);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.2375);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::istore, 3);
		code.add(Opcode::iconst, 249);
		code.add(Opcode::istore, 4);
		code.add(Opcode::dload, 4);
		code.add(Opcode::dstore, 5);
		code.add(Opcode::iload, 0);
		code.add(Opcode::dload, 1);
		code.add(Opcode::iload, 3);
		code.add(Opcode::dload, 5);

		FunctionTable table;
		table.add(6, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popDouble() == (double)249);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popDouble() == (double)37);
		ASSERT(ops.popInt() == 3);
	}

	// 整数四则运算
	{
		Code code;
		code.add(Opcode::iconst, 2);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::iadd);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::imul);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::idiv);
		code.add(Opcode::iconst, 7);
		code.add(Opcode::isub);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 3);
	}

	// 浮点数四则运算
	{
		Code code;
		code.add(Opcode::dconst, 2.75);
		code.add(Opcode::dconst, 3.6);
		code.add(Opcode::dconst, 9.1);
		code.add(Opcode::dmul);
		code.add(Opcode::dadd);
		code.add(Opcode::dconst, 6.5);
		code.add(Opcode::ddiv);
		code.add(Opcode::dconst, 3.8);
		code.add(Opcode::dsub);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popDouble() == (2.75 + 3.6 * 9.1) / 6.5 - 3.8);
	}

	// 整数和浮点数混合四则运算
	{
		Code code;
		code.add(Opcode::iconst, 6);
		code.add(Opcode::dconst, 2.3);
		code.add(Opcode::idiv);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::dconst, 2.3);
		code.add(Opcode::ddiv);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::idiv);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::ddiv);
		code.add(Opcode::dstore, 3);
		code.add(Opcode::iload, 0);
		code.add(Opcode::dload, 1);
		code.add(Opcode::iload, 2);
		code.add(Opcode::dload, 3);

		FunctionTable table;
		table.add(4, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popDouble() == 2.0 / 3.0);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popDouble() == 6 / 2.3);
		ASSERT(ops.popInt() == 6 / 2);
	}

	// 整数除零
	{
		Code code;
		code.add(Opcode::iconst, 3);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::idiv);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		
		ASSERT_EXCEPTION(vm.exec());
	}

	// 浮点数除零
	{
		Code code;
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dconst, 0.0);
		code.add(Opcode::ddiv);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);

		ASSERT_EXCEPTION(vm.exec());
	}

	// icmp 整数比较
	{
		Code code;
		code.add(Opcode::iconst, -1);
		code.add(Opcode::iconst, 30);
		code.add(Opcode::icmp);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 25);
		code.add(Opcode::iconst, 25);
		code.add(Opcode::icmp);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::iconst, 8);
		code.add(Opcode::icmp);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == -1);
	}

	// dcmp 浮点数比较
	{
		Code code;
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dconst, 3.15);
		code.add(Opcode::dcmp);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 23.75);
		code.add(Opcode::dconst, 23.75);
		code.add(Opcode::dcmp);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 23423.435);
		code.add(Opcode::dconst, 4.68);
		code.add(Opcode::dcmp);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 0);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == -1);
	}

	// 整数与浮点数混合比较
	{
		Code code;
		code.add(Opcode::iconst, 3);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::icmp);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dcmp);
		code.add(Opcode::istore, 1);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::dcmp);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == -1);
		ASSERT(ops.popInt() == 0);
	}

	// ig
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::ig);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::ig);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::ig);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
	}

	// ige
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::ige);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::ige);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::ige);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
	}

	// il
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::il);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::il);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::il);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
	}

	// ile
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::ile);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::ile);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::ile);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
	}

	// ie
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::ie);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::ie);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::ie);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
	}

	// ine
	{
		Code code;
		code.add(Opcode::iconst, 13);
		code.add(Opcode::iconst, 27);
		code.add(Opcode::ine);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::iconst, 50);
		code.add(Opcode::ine);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 79);
		code.add(Opcode::iconst, 6);
		code.add(Opcode::ine);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
	}

	// iand
	{
		Code code;
		code.add(Opcode::iconst, 2725);
		code.add(Opcode::iconst, 66358);
		code.add(Opcode::iand);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 548);
	}

	// ior
	{
		Code code;
		code.add(Opcode::iconst, 2725);
		code.add(Opcode::iconst, 66358);
		code.add(Opcode::ior);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 68535);
	}

	// inot
	{
		Code code;
		code.add(Opcode::iconst, 2725);
		code.add(Opcode::inot);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == -2726);
	}

	// dg
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::dg);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dg);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::dg);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
	}

	// dge
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::dge);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dge);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::dge);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
	}

	// dl
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::dl);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dl);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::dl);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
	}

	// dle
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::dle);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dle);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::dle);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
	}

	// de
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::de);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::de);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::de);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
	}

	// dne
	{
		Code code;
		code.add(Opcode::dconst, 1.6);
		code.add(Opcode::dconst, 27.34);
		code.add(Opcode::dne);
		code.add(Opcode::dstore, 0);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dconst, 332.454);
		code.add(Opcode::dne);
		code.add(Opcode::dstore, 1);
		code.add(Opcode::dconst, 0.778);
		code.add(Opcode::dconst, 0.726);
		code.add(Opcode::dne);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dload, 2);
		code.add(Opcode::dload, 1);
		code.add(Opcode::dload, 0);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
	}

	// jmp
	{
		Code code;
		code.add(Opcode::iconst, 2);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 5);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(1, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 2);
	}

	// jl
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::jl, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::jl, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::jl, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 500);
		ASSERT(ops.popInt() == 300);
		ASSERT(ops.popInt() == 200);
	}

	// jle
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::jle, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::jle, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::jle, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 500);
		ASSERT(ops.popInt() == 400);
		ASSERT(ops.popInt() == 200);
	}

	// jg
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::jg, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::jg, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::jg, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 600);
		ASSERT(ops.popInt() == 300);
		ASSERT(ops.popInt() == 100);
	}

	// jge
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::jge, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::jge, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::jge, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 600);
		ASSERT(ops.popInt() == 400);
		ASSERT(ops.popInt() == 100);
	}

	// je
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::je, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::je, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::je, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 500);
		ASSERT(ops.popInt() == 400);
		ASSERT(ops.popInt() == 100);
	}

	// jne
	{
		Code code;
		code.add(Opcode::iconst, -1); // 0
		code.add(Opcode::jne, 5);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::istore, 0);
		code.add(Opcode::jmp, 7);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 0); // 7
		code.add(Opcode::jne, 12);
		code.add(Opcode::iconst, 300);
		code.add(Opcode::istore, 1);
		code.add(Opcode::jmp, 14);
		code.add(Opcode::iconst, 400);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1); // 14
		code.add(Opcode::jne, 19);
		code.add(Opcode::iconst, 500);
		code.add(Opcode::istore, 2);
		code.add(Opcode::jmp, 21);
		code.add(Opcode::iconst, 600);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iload, 0); // 21
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);

		FunctionTable table;
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 600);
		ASSERT(ops.popInt() == 300);
		ASSERT(ops.popInt() == 200);
	}

	// 循环测试
	{
		Code code;
		code.add(Opcode::iconst, 0);
		code.add(Opcode::istore, 0); // s = 0
		code.add(Opcode::iconst, 1);
		code.add(Opcode::istore, 1); // i = 1
		code.add(Opcode::iload, 1);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::icmp); 
		code.add(Opcode::jg, 16); // i > 100 ?
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iadd);
		code.add(Opcode::istore, 0); // s = s + i
		code.add(Opcode::iload, 1);
		code.add(Opcode::inc);
		code.add(Opcode::istore, 1); // i++
		code.add(Opcode::jmp, 4);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(2, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 5050);
	}

	// 函数调用1
	{
		Code code;
		code.add(Opcode::iconst, 25); // function main
		code.add(Opcode::call, 1); // foo(25)
		code.add(Opcode::ret);
		code.add(Opcode::istore, 0); // function foo
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 0);
		code.add(Opcode::imul);
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 0);
		table.add(1, 3);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 625);
	}

	// 函数调用2
	{
		Code code;
		code.add(Opcode::istore, 0); // int sum(int n)
		code.add(Opcode::iconst, 0);
		code.add(Opcode::istore, 1); // int s = 0
		code.add(Opcode::iconst, 1);
		code.add(Opcode::istore, 2); // int i = 1
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 0);
		code.add(Opcode::icmp);
		code.add(Opcode::jg, 17); // i > n?
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iadd);
		code.add(Opcode::istore, 1); // s += i
		code.add(Opcode::iload, 2);
		code.add(Opcode::inc);
		code.add(Opcode::istore, 2); // i++
		code.add(Opcode::jmp, 5);
		code.add(Opcode::iload, 1);
		code.add(Opcode::ret); // return s
		code.add(Opcode::iconst, 100); // void main()
		code.add(Opcode::call, 1); // sum(100);
		code.add(Opcode::iconst, 2378);
		code.add(Opcode::call, 1); // sum(1000);
		code.add(Opcode::ret); // return

		FunctionTable table;
		table.add(0, 19);
		table.add(3, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 2828631);
		ASSERT(ops.popInt() == 5050);
	}

	// 函数调用3
	{
		Code code;
		code.add(Opcode::istore, 0);
		code.add(Opcode::istore, 1); // int pow(int a, int b)
		code.add(Opcode::iconst, 1);
		code.add(Opcode::istore, 2); // r = 1
		code.add(Opcode::iconst, 1);
		code.add(Opcode::istore, 3); // i = 1
		code.add(Opcode::iload, 3);
		code.add(Opcode::iload, 1);
		code.add(Opcode::ile); // i <= b?
		code.add(Opcode::je, 18);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 0);
		code.add(Opcode::imul);
		code.add(Opcode::istore, 2); // r *= a
		code.add(Opcode::iload, 3);
		code.add(Opcode::inc);
		code.add(Opcode::istore, 3); // i++
		code.add(Opcode::jmp, 6);
		code.add(Opcode::iload, 2);
		code.add(Opcode::ret); // return r
		code.add(Opcode::iconst, 6); // void main()
		code.add(Opcode::iconst, 17);
		code.add(Opcode::call, 1); // pow(17, 6)
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 20);
		table.add(4, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 24137569);
	}

	// 函数调用4
	{
		Code code;
		code.add(Opcode::istore, 0); // int fac(int n)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::ie); // n == 0 ?
		code.add(Opcode::jne, 6);
		code.add(Opcode::jmp, 8);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ret); // return 1
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fac(n - 1)
		code.add(Opcode::imul);
		code.add(Opcode::ret); // return n * fac(n - 1)
		code.add(Opcode::iconst, 5); // void main()
		code.add(Opcode::call, 1); // fac(5)
		code.add(Opcode::iconst, 10);
		code.add(Opcode::call, 1); // fac(10)
		code.add(Opcode::iadd); // fac(5) + fac(10)
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 15);
		table.add(1, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 3628920);
	}

	// 函数调用5
	{
		Code code;
		code.add(Opcode::iconst, 20); // void main()
		code.add(Opcode::call, 1); // fib(20)
		code.add(Opcode::iconst, 23);
		code.add(Opcode::call, 1); // fib(23)
		code.add(Opcode::iadd); // fib(20) + fib(23)
		code.add(Opcode::ret);
		code.add(Opcode::istore, 0); // int fib(int n)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ie); // n == 1 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::ie); // n == 2 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::jmp, 18);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ret); // return 1
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 1)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 2)
		code.add(Opcode::iadd);
		code.add(Opcode::ret); // return fib(n - 1) + fib(n - 2)

		FunctionTable table;
		table.add(0, 0);
		table.add(1, 6);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 35422);
	}

	// 异常测试1
	{
		Code code;
		code.add(Opcode::iconst, 20); // void main()
		code.add(Opcode::call, 1); // fib(20)
		code.add(Opcode::iconst, 23);
		code.add(Opcode::call, 1); // fib(23)
		code.add(Opcode::iadd); // fib(20) + fib(23)
		code.add(Opcode::ret);
		code.add(Opcode::istore, 0); // int fib(int n)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ie); // n == 1 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::ie); // n == 2 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::jmp, 18);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ret); // return 1
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 1)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 2)
		code.add(Opcode::iadd);
		code.add(Opcode::ret); // return fib(n - 1) + fib(n - 2)

		FunctionTable table;
		table.add(0, 0);
		table.add(0, 6);

		ByxVM vm(code, table, 0);
		ASSERT_EXCEPTION(vm.exec());
	}

	// 异常测试2
	{
		Code code;
		code.add(Opcode::iconst, 20); // void main()
		code.add(Opcode::call, 2); // fib(20)
		code.add(Opcode::iconst, 23);
		code.add(Opcode::call, 1); // fib(23)
		code.add(Opcode::iadd); // fib(20) + fib(23)
		code.add(Opcode::ret);
		code.add(Opcode::istore, 0); // int fib(int n)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ie); // n == 1 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::ie); // n == 2 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::jmp, 18);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ret); // return 1
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 1)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 2)
		code.add(Opcode::iadd);
		code.add(Opcode::ret); // return fib(n - 1) + fib(n - 2)

		FunctionTable table;
		table.add(0, 0);
		table.add(1, 6);

		ByxVM vm(code, table, 0);
		ASSERT_EXCEPTION(vm.exec());
	}

	// 异常测试3
	{
		Code code;
		code.add(Opcode::iconst, 20); // void main()
		code.add(Opcode::iload, 0);
		code.add(Opcode::call, 1); // fib(20)
		code.add(Opcode::iconst, 23);
		code.add(Opcode::call, 1); // fib(23)
		code.add(Opcode::iadd); // fib(20) + fib(23)
		code.add(Opcode::ret);
		code.add(Opcode::istore, 0); // int fib(int n)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ie); // n == 1 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::ie); // n == 2 ?
		code.add(Opcode::jne, 16);
		code.add(Opcode::jmp, 18);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::ret); // return 1
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 1)
		code.add(Opcode::iload, 0);
		code.add(Opcode::iconst, 2);
		code.add(Opcode::isub);
		code.add(Opcode::call, 1); // fib(n - 2)
		code.add(Opcode::iadd);
		code.add(Opcode::ret); // return fib(n - 1) + fib(n - 2)

		FunctionTable table;
		table.add(1, 0);
		table.add(1, 6);

		ByxVM vm(code, table, 0);
		ASSERT_EXCEPTION(vm.exec());
	}

	// 全局变量测试
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::iconst, 10);
		code.add(Opcode::iadd);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::dconst, 2.0);
		code.add(Opcode::dmul);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 2);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popDouble() == 3.14 * 2);
		ASSERT(ops.popInt() == 110);
	}

	// 异常测试4
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::iconst, 10);
		code.add(Opcode::iadd);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::dconst, 2.0);
		code.add(Opcode::dmul);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		
		ASSERT_EXCEPTION(vm.exec());
	}

	// 异常测试5
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::iconst, 10);
		code.add(Opcode::iadd);
		code.add(Opcode::igstore, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::dconst, 2.0);
		code.add(Opcode::dmul);
		code.add(Opcode::dgstore, 1);
		code.add(Opcode::igload, 0);
		code.add(Opcode::dgload, 1);
		code.add(Opcode::ret);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 1);

		ASSERT_EXCEPTION(vm.exec());
	}

	// toi tod
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::toi);
		code.add(Opcode::istore, 0);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::toi);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 100);
		code.add(Opcode::tod);
		code.add(Opcode::dstore, 2);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::tod);
		code.add(Opcode::dstore, 3);
		code.add(Opcode::dload, 3);
		code.add(Opcode::dload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(4, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 100);
		ASSERT(ops.popInt() == 3);
		ASSERT(ops.popDouble() == 100.0);
		ASSERT(ops.popDouble() == 3.14);
	}

	// pop
	{
		Code code;
		code.add(Opcode::iconst, 100);
		code.add(Opcode::iconst, 200);
		code.add(Opcode::dconst, 3.14);
		code.add(Opcode::pop);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 200);
		ASSERT(ops.popInt() == 100);
		ASSERT_EXCEPTION(ops.popInt());
	}

	// rem
	{
		Code code;
		code.add(Opcode::iconst, 3);
		code.add(Opcode::iconst, 10);
		code.add(Opcode::rem);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 10);
		code.add(Opcode::iconst, 3);
		code.add(Opcode::rem);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iload, 0);
		code.add(Opcode::iload, 1);

		FunctionTable table;
		table.add(2, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 3);
	}

	// rem异常
	{
		Code code;
		code.add(Opcode::iconst, 3);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::rem);

		FunctionTable table;
		table.add(0, 0);

		ByxVM vm(code, table, 0);
		ASSERT_EXCEPTION(vm.exec());
	}

	// land
	{
		Code code;
		code.add(Opcode::iconst, 0);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::land);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::land);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::land);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::land);
		code.add(Opcode::istore, 3);
		code.add(Opcode::iload, 3);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(4, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
	}

	// lor
	{
		Code code;
		code.add(Opcode::iconst, 0);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::lor);
		code.add(Opcode::istore, 0);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::lor);
		code.add(Opcode::istore, 1);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 0);
		code.add(Opcode::lor);
		code.add(Opcode::istore, 2);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::iconst, 1);
		code.add(Opcode::lor);
		code.add(Opcode::istore, 3);
		code.add(Opcode::iload, 3);
		code.add(Opcode::iload, 2);
		code.add(Opcode::iload, 1);
		code.add(Opcode::iload, 0);

		FunctionTable table;
		table.add(4, 0);

		ByxVM vm(code, table, 0);
		vm.exec();
		OperandStack ops = vm.getOpStack();

		ASSERT(ops.popInt() == 0);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
		ASSERT(ops.popInt() == 1);
	}

	watch.end();
	cout << "ByxVM test passed. time: " << watch.duration() << "s" << endl;
}