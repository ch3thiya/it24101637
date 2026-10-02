// message2_bsend.cc
// mpicxx message2_bsend.cc -o message2_bsend && mpirun -np 2 ./message2_bsend
#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Setup buffer for MPI_Bsend
    int buffer_size;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &buffer_size);
    // Multiply by 3 for the loop, and add MPI_BSEND_OVERHEAD per message
    int total_buffer_size = 3 * (buffer_size + MPI_BSEND_OVERHEAD);
    void* buffer = malloc(total_buffer_size);
    
    // Attach the buffer to MPI
    MPI_Buffer_attach(buffer, total_buffer_size);

    int number;
    for (int i = 0; i < 3; i++) {
        if (rank == 0) {
            number = i * 10;
            // Changed from MPI_Send to MPI_Bsend
            MPI_Bsend(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            std::cout << "Process 0 sent " << number << "\n";
        } else if (rank == 1) {
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process 1 received " << number << "\n";
        }
    }

    // Detach buffer and free memory
    MPI_Buffer_detach(&buffer, &total_buffer_size);
    free(buffer);

    MPI_Finalize();
    return 0;
}
