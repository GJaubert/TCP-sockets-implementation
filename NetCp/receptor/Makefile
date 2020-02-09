DFA2dot: main.o socket_.o
	g++ main.o socket_.o -std=c++11 -o Sockete -pthread
main.o: main.cpp
	g++ -c main.cpp
socket_.o: socket_.h socket_.cpp
	g++ -c socket_.cpp
DFAWriter.o: DFAWriter.cpp DFAWriter.h
	g++ -c DFAWriter.cpp
clean:
	rm *.o Sockete 
