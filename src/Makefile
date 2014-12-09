
build: build_OMP build_THREADS build_MPI

build_OMP: Graph.o
	g++ -fopenmp -std=c++11 Versions/OMPVersion.cpp Graph.o -o OMP

build_THREADS: Graph.o
	g++ Versions/ThreadsVersion.cpp -o THREADS -pthread -fopenmp -std=c++11

build_MPI: Graph.o
	mpicxx -std=c++11 Versions/MPIVersion.cpp -o MPI -fopenmp

Graph.o:
	g++ -fopenmp -std=c++11 -c Core/Graph.cpp -o Graph.o

build_run: clean build_OMP build_MPI build_THREADS
	./THREADS
	mpirun -n 8 ./MPI
	./OMP

clean:
	rm -f OMP
	rm -f MPI
	rm -f THREADS
	rm -f *.o