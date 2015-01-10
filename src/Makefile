
build: build_OMP build_THREADS build_MPI

build_OMP: Graph.o
	g++ -fopenmp -std=c++11 Versions/OMPVersion.cpp Graph.o -o OMP

build_THREADS: Graph.o
	g++ -fopenmp -std=c++11 Versions/ThreadsVersion.cpp Graph.o -lpthread -o THREADS 

build_MPI: Graph.o
	mpicxx -fopenmp -std=c++11 Versions/MPIVersion.cpp Graph.o -o MPI 

Graph.o:
	g++ -fopenmp -std=c++11 -c Core/Graph.cpp -o Graph.o

run_OMP:
	./OMP

run_THREADS:
	./THREADS

run_MPI:
	mpirun -np 8 ./MPI

build_run: clean build_OMP build_MPI build_THREADS
	./THREADS
	mpirun -n 8 ./MPI
	./OMP

clean:
	rm -f OMP
	rm -f MPI
	rm -f THREADS
	rm -f *.o

plot:
	gnuplot -persist -e "set ylabel 'Num threads'; set xlabel 'Time (sec)'; plot 'mpi_out' using 2:1 title 'MPI' with lines, 'thread_out' using 2:1 title 'THREAD' with lines,'omp_out' using 2:1 title 'OMP' with lines; set terminal png; set out 'opt.png';replot ;set term pop";
