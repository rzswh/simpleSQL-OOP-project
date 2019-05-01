.PHONY: clean

all: main

OBJ = WhereClause.o Table.o Value.o Attribute.o PrintableTable.o UnitTest.o
HEADER = WhereClause.h Table.h Value.h Attribute.h Record.h

main: main.cpp $(HEADER) $(OBJ)
	g++ -o main main.cpp $(OBJ)

Value.o: Value.cpp Value.h
	g++ -c $*.cpp -o $*.o

Attribute.o: Attribute.cpp Attribute.h
	g++ -c $*.cpp -o $*.o

WhereClause.o: WhereClause.cpp WhereClause.h Value.h
	g++ -c $*.cpp -o $*.o

Table.o: Table.cpp Attribute.h WhereClause.h Value.h Record.h
	g++ -c $*.cpp -o $*.o

PrintableTable.o: PrintableTable.cpp Table.h Attribute.h WhereClause.h Value.h Record.h
	g++ -c $*.cpp -o $*.o

UnitTest.o: UnitTest.cpp UnitTest.h $(HEADER)
	g++ -c $*.cpp -o $*.o

clean:
	rm *.o
	rm main