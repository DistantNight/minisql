#include <iostream>
#include <string>
#include <vector>
#include "OperationInput.h"
#include "API.h"
#include "InputProcess.h"
#include "catalog.h"
#include "buffer.h"
using namespace std;
catalogManager x;
Buffer* database;
int main()
{
    Buffer d;
    database = &d;
	logInterface();
	execInterface();
	
    x.updateCatalog();
	system("pause");
	return 0;
}

