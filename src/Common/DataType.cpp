#include "DataType.h"

std::string DataTypeToString(DataType type)
{
	if (type == DataType::Integer)
	{
		return "int";
	}
	else if (type == DataType::Double)
	{
		return "double";
	}
	else if (type == DataType::Void)
	{
		return "void";
	}
	else
	{
		return "unknown";
	}
}