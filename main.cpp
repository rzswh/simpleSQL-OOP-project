#include<iostream>
#include"Interpreter.h"
#include "UnitTest.h"
using namespace std;

int main()
{
	unitTests();
	return 0;
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
