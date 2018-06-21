#include <iostream>
#include <string>
#include <vector>
#include "OperationInput.h"
#include "API.h"
#include "InputProcess.h"
#include "catalog.h"
#include "buffer.h"

using namespace std;
catalogManager *catalog_manager = nullptr;
Buffer* database = nullptr;

int main()
{
    Buffer d;
    database = &d;
    catalogManager c;
    catalog_manager = &c;

	logInterface(); // user: root, password: 456123asd
	execInterface();
	
    c.updateCatalog();
	system("pause");
	return 0;
}

