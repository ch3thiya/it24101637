#include <mpi.h>
#include <iostream>
#include <random>
#include <cstdlib>

int main(int argc, char* argv[]) {
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
        for (int i = 1; i < size; i++) {
            MPI_Recv(&received_in_circle, 1, MPI_LONG_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_in_circle += received_in_circle;
        }

        double pi_estimate = 4.0 * global_in_circle / (double)total_tosses;
        std::cout << "Estimated value of Pi after " << total_tosses << " iterations: " << pi_estimate << "\n";
    } else {
        // Setup buffer for MPI_Bsend
        int buffer_size;
        MPI_Pack_size(1, MPI_LONG_LONG, MPI_COMM_WORLD, &buffer_size);
        int total_buffer_size = buffer_size + MPI_BSEND_OVERHEAD;
        void* buffer = malloc(total_buffer_size);
        
        // Attach the buffer to MPI
        MPI_Buffer_attach(buffer, total_buffer_size);

        // Send local count to rank 0 using MPI_Bsend
        MPI_Bsend(&local_in_circle, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);

        // Detach buffer and free memory
        MPI_Buffer_detach(&buffer, &total_buffer_size);
        free(buffer);
    }

    MPI_Finalize();
    return 0;
}
