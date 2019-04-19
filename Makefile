.PHONY: clean

all: main

main:
	ls

Value.o: Value.cpp Value.h
	g++ -c $*.cpp -o $*.o

Attribute.o: Attribute.cpp Attribute.h
	g++ -c $*.cpp -o $*.o

WhereClause.o: WhereClause.cpp WhereClause.h Value.h
	g++ -c $*.cpp -o $*.o

Table.o: Table.cpp Attribute.h WhereClause.h Value.h Record.h
	g++ -c $*.cpp -o $*.o

clean:
	rm *.o