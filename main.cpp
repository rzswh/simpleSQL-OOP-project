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
	//cout << (Value<int>(4) == Value<double>(4.0)) << endl;
	return 0;
}
