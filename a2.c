#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int numberOfProcesses;
    int currentRank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    printf("rank: %d\n\n", currentRank);

    MPI_Finalize();
}