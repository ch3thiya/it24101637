#include <mpi.h>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long total_tosses = 10000000;
    
    // Distribute iterations evenly, handling any remainder for the last process
    long long local_tosses = total_tosses / size;
    if (rank == size - 1) {
        local_tosses += total_tosses % size;
    }

    // Use a unique seed for each process using its rank
    std::mt19937_64 gen(rank + 12345); 
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    long long local_in_circle = 0;
    for (long long i = 0; i < local_tosses; i++) {
        double x = dist(gen);
        double y = dist(gen);
        
        if (x * x + y * y <= 1.0) {
            local_in_circle++;
        }
    }

    long long global_in_circle = 0;
    
    // Reduce all local counts into a global count at rank 0
    MPI_Reduce(&local_in_circle, &global_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi_estimate = 4.0 * global_in_circle / (double)total_tosses;
        std::cout << "Estimated value of Pi after " << total_tosses << " iterations: " << pi_estimate << "\n";
    }

    MPI_Finalize();
    return 0;
}
