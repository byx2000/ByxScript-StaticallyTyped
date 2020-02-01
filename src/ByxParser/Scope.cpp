#include "Scope.h"

using namespace std;

void Scope::define(const std::string& name, SymbolInfo symbol)
{
	if (symbolTable.count(name) > 0)
	{
		throw SymbolRedefined();
	}
	symbolTable[name] = symbol;
}

SymbolInfo Scope::resolve(const std::string& name)
{
	if (symbolTable.count(name) == 0)
	{
		throw SymbolNotFound();
	}
	return symbolTable[name];
}

int Scope::getSymbolCount() const
{
	return symbolTable.size();
}
