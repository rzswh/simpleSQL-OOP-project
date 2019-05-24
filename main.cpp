#include<iostream>
#include"Interpreter.h"
using namespace std;

int main()
{
	Interpreter interpreter;
	char *sqlsentence = new char[1024];
	while (true)
	{
		cin.getline(sqlsentence,1024);
        if (!cin) break;
		interpreter.SQL(sqlsentence);
	}
	//cout << (Value<int>(4) == Value<double>(4.0)) << endl;
	delete [] sqlsentence;
	return 0;
}
