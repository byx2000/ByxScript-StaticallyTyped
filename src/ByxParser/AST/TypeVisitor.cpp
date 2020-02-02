#include "TypeVisitor.h"

void TypeVisitor::visit(BinaryOpNode& node)
{
	node.lhs->visit(*this);
	node.rhs->visit(*this);
	if (node.lhs->dataType == DataType::Double || node.rhs->dataType == DataType::Double)
	{
		node.dataType = DataType::Double;
	}
	else
	{
		node.dataType = DataType::Integer;
	}
}
