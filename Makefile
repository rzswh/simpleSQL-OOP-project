.PHONY: clean debug test

all: main

OBJ = WhereClause.o DataBase.o Table.o Value.o Attribute.o PrintableTable.o sql.o Interpreter.o manager.o Expression.o
HEADER = WhereClause.h DataBase.h Table.h Value.h Attribute.h sql.h Interpreter.h Record.h manager.h PrintableTable.h Expression.h
ARGS = -std=c++11 -g

main: main.cpp $(HEADER) $(OBJ)
	g++ -o main main.cpp $(OBJ) $(ARGS)

debug: main.cpp $(HEADER) $(OBJ) main
	g++ -o main main.cpp $(OBJ) $(ARGS) -g

test: main.cpp $(HEADER) $(OBJ) main
	g++ -o main main.cpp $(OBJ) test/UnitTest.cpp $(ARGS) -g -DTEST

Value.o: Value.cpp Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Attribute.o: Attribute.cpp Attribute.h
	g++ -c $*.cpp -o $*.o $(ARGS)

DataBase.o: DataBase.cpp DataBase.h Table.h sql.h Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Interpreter.o: Interpreter.cpp Interpreter.h manager.h sql.h Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

manager.o: manager.cpp manager.h sql.h DataBase.h
	g++ -c $*.cpp -o $*.o $(ARGS)

PrintableTable.o: PrintableTable.cpp PrintableTable.h Attribute.h Value.h Record.h  Expression.h
	g++ -c $*.cpp -o $*.o $(ARGS)

sql.o: sql.cpp sql.h manager.h Expression.h Attribute.h WhereClause.h Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

WhereClause.o: WhereClause.cpp WhereClause.h Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Table.o: Table.cpp Table.h Attribute.h WhereClause.h Value.h Record.h PrintableTable.h Expression.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Expression.o: Expression.cpp Expression.h Attribute.h Value.h Record.h
	g++ -c $*.cpp -o $*.o $(ARGS)

clean:
	rm *.o
	rm main