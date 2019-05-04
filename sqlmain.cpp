#include<iostream>
#include"Interpreter.h"
using namespace std;

int main()
{
	Interpreter interpreter;
	char sqlsentence[200];
	cin.getline(sqlsentence,200);
	interpreter.SQL(sqlsentence);
	return 0;
}
