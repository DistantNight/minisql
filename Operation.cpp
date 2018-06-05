#include "Operation.h"

Operation::Operation()
{
	operation_type = 0;
}

Operation::~Operation()
{

}

int Operation::getType()
{
	return operation_type;
}

void Operation::setType(int t)
{
	operation_type = t;
}

bool Operation::isError()
{
	if (0 < this->getType())
		return true;
	return false;
}

