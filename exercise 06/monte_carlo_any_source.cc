#include <mpi.h>
#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
    double start_time = MPI_Wtime();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long total_tosses = 10000000;
    
    long long local_tosses = total_tosses / size;
    if (rank == size - 1) {
        local_tosses += total_tosses % size;
    }

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

    if (rank == 0) {
        global_in_circle = local_in_circle;
        long long received_in_circle;
        
        // Receive from all other processes using MPI_ANY_SOURCE
        // This allows rank 0 to receive the message from whichever process finishes first,
        // rather than waiting in a strict sequential order (e.g. rank 1, then rank 2, etc.)
        for (int i = 1; i < size; i++) {
            MPI_Recv(&received_in_circle, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_in_circle += received_in_circle;
        }

        double pi_estimate = 4.0 * global_in_circle / (double)total_tosses;
        std::cout << "Estimated value of Pi after " << total_tosses << " iterations: " << pi_estimate << "\n";
    } else {
        // Send local count to rank 0
        MPI_Send(&local_in_circle, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
