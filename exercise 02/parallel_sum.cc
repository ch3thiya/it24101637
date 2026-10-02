#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = 10000000;
    long long chunk_size = N / size;
    long long start = rank * chunk_size + 1;
    long long end = (rank == size - 1) ? N : (rank + 1) * chunk_size;

    long long local_sum = 0;
    for (long long i = start; i <= end; ++i) {
        local_sum += i;
    }

    long long global_sum = 0;
    
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "The sum of numbers from 1 to " << N << " is " << global_sum << "\n";
    }

    MPI_Finalize();
    return 0;
}
