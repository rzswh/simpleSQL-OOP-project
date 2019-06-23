.PHONY: clean

all: main1 main2

OBJ = DataBase.o Table.o Value.o Attribute.o PrintableTable.o sql.o Interpreter.o manager.o Expression.o
HEADER = DataBase.h Table.h Value.h Attribute.h sql.h Interpreter.h Record.h manager.h PrintableTable.h Expression.h
ARGS = -std=c++11

main1: main.cpp $(HEADER) $(OBJ)
	g++ -o main1 main.cpp $(OBJ) $(ARGS)

main2: main.cpp $(HEADER) $(OBJ)
	g++ -o main2 main.cpp $(OBJ) $(ARGS)
	
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

sql.o: sql.cpp sql.h manager.h Expression.h Attribute.h Value.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Table.o: Table.cpp Table.h Attribute.h Value.h Record.h PrintableTable.h Expression.h
	g++ -c $*.cpp -o $*.o $(ARGS)

Expression.o: Expression.cpp Expression.h Attribute.h Value.h Record.h
	g++ -c $*.cpp -o $*.o $(ARGS)

clean:
	rm *.o
	rm main1 main2