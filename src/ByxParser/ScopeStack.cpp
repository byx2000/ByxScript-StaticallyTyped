#include "ScopeStack.h"

using namespace std;

ScopeStack::ScopeStack()
{
	symbolsCount = 0;
}

void ScopeStack::pushScope()
{
	scopes.push_back(Scope());
}

void ScopeStack::popScope()
{
	if (scopes.size() == 0)
	{
		throw EmptyStack();
	}

	scopes.pop_back();
}

int ScopeStack::define(const std::string& name, DataType type)
{
	if (scopes.size() == 0)
	{
		throw EmptyStack();
	}

	try
	{
			scopes[(int)scopes.size() - 1].define(name, SymbolInfo(type, symbolsCount, false));
			symbolsCount++;
			return symbolsCount - 1;
	}
	catch (Scope::SymbolRedefined)
	{
		throw SymbolRedefined();
	}
}

SymbolInfo ScopeStack::resolve(const std::string& name)
{
	for (int i = scopes.size() - 1; i >= 0; --i)
	{
		try
		{
			SymbolInfo symbol = scopes[i].resolve(name);
			return symbol;
		}
		catch (Scope::SymbolNotFound)
		{
			continue;
		}
	}
	throw SymbolNotFound();
}

int ScopeStack::getSpace() const
{
	return symbolsCount;
}
