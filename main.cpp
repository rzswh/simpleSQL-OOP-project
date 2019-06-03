#include<iostream>
#include"Interpreter.h"
#ifdef TEST
#include "test/UnitTest.h"
#endif
using namespace std;

int main()
{
#ifdef TEST
	unitTests();
	return 0;
#endif
	Interpreter interpreter;
	char *sqlsentence = new char[1024];
	while (true)
	{
		cin.getline(sqlsentence,1024);
        if (!cin) break;
		interpreter.SQL(sqlsentence);
	}
	delete [] sqlsentence;
	return 0;
}
