
build: clean Graph.o SerialSolution.o
	g++ -std=c++11 Main.cpp Graph.o SerialSolution.o -o Main

Graph.o:
	g++ -std=c++11 -c Util/Graph.cpp -o Graph.o

SerialSolution.o:
	g++ -std=c++11 -c Versions/SerialSolution.c -o SerialSolution.o

build_run: clean build
	./Main

run:
	./Main

clean:
	rm -f Main
	rm -f *.o
