//#include "UnitTest.h"
#include<iostream>
#include"Interpreter.h"
using namespace std;
/*
// for unit test
int unit_test_main () {
    whereClauseUnitTest();
    tableUnitTest();
    return 0;
}*/

int main()
{
	Interpreter interpreter;
	char sqlsentence[200];
	while (true)
	{
		cin.getline(sqlsentence,200);
        if (!cin) break;
		interpreter.SQL(sqlsentence);
	}
	return 0;
}
