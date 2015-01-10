cd src
rm mpi_out
rm omp_out
rm thread_out
make clean
make build

GSIZE=500

for VARIABLE in 1 2 3 4 5 6 7 8
do

	/usr/bin/time -f "$VARIABLE, %e" --append -o omp_out ./OMP $VARIABLE $GSIZE
	/usr/bin/time -f "$VARIABLE, %e" --append -o thread_out ./THREADS $VARIABLE $GSIZE
	/usr/bin/time -f "$VARIABLE, %e" --append -o mpi_out mpirun -n $VARIABLE ./MPI $GSIZE
done

make plot