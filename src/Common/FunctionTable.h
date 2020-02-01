#pragma once

#include <string>
#include <vector>

class FunctionEntry
{
public:
	FunctionEntry(int space, int addr);
	std::string toString() const;

	int space; // �������õĿռ��С
	int addr; // ������ʼ��ַ
};

class FunctionTable
{
public:
	FunctionTable(const std::vector<FunctionEntry>& table = std::vector<FunctionEntry>());
	void add(int space, int addr); // ��Ӻ���
	int getSpace(int index) const; // ��ȡ�������õĿռ��С
	int getAddr(int index) const; // ��ȡ��������ʼ��ַ
	const FunctionEntry& getAllInfo(int index) const; // ��ȡ������Ϣ
	int getCount() const; // ��ȡ��������

	void setAddr(int index, int addr); // ���ú�����ַ
	void setSpace(int index, int space); // ���ú����ռ�

	std::string toString() const;

	// �쳣
	class FunctionIndexOutOfBoundary {}; // �����±�Խ��

private:
	std::vector<FunctionEntry> table; // ������Ϣ��
	void checkIndex(int index) const; // ��麯���±�
};