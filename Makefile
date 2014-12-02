
build: Graph.o
	g++ -std=c++11 Main.cpp Graph.o -o Main

build_OMP: clean Graph.o
	g++ -fopenmp -std=c++11 Main.cpp Graph.o -o Main

Graph.o:
	g++ -fopenmp -std=c++11 -c Graph.cpp -o Graph.o

build_run: clean build
	./Main

run:
	./Main

clean:
	rm -f Main
	rm -f *.o