#include <stdio.h>
#include <mpi.h>

int edge(int rank) {
    if (rank == 1 ||
        rank == 2 ||
        rank == 3 ||
        rank == 4 ||
        rank == 5 ||
        rank == 6 ||
        rank == 10 ||
        rank == 11 ||
        rank == 15 ||
        rank == 16 ||
        rank == 17 ||
        rank == 18 ||
        rank == 19 ||
        rank == 20
    ) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char **argv) {
    int numberOfProcesses;
    int currentRank;
    int masterRank = 0;
    int leftNeighbourTag = 0;
    int rightNeighbourTag = 1;
    int topNeighbourTag = 2;
    int bottomNeighbourTag = 3;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    if (currentRank == masterRank) {

    } else {
        // generate random number
        // send random number to adjacent neighbours
        // if this process is NOT a corner process, block recieve to check neighbours
        int randomNumber = 1;
        int leftNeighbour = currentRank - 1;
        int rightNeighbour = currentRank + 1;
        int topNeighbour = currentRank - 5;
        int bottomNeighbour = currentRank + 5;
        MPI_Request leftRequest;
        MPI_Request rightRequest;
        MPI_Request topRequest;
        MPI_Request bottomRequest;
        // we dont want outside edges to send to neighbours outside the bounds
        if (leftNeighbour > 0 &&
            currentRank != 1 && // this is the left side of the grid
            currentRank != 5 && // we dont want to send to our left neighbour 
            currentRank != 11 && // because we dont have a left neighbour
            currentRank != 16) {
            printf("rank: %d, left: %d\n\n", currentRank, leftNeighbour);
            MPI_Isend(&randomNumber, 1, MPI_INT, leftNeighbour, leftNeighbourTag, MPI_COMM_WORLD, &leftRequest);
        }
        if (rightNeighbour < 21 &&
            currentRank != 5 && // this is the right side of the grid
            currentRank != 10 && // we dont want to send to our right neighbour 
            currentRank != 15 && // because we dont have a right neighbour
            currentRank != 20) {
            printf("rank: %d, right: %d\n\n", currentRank, rightNeighbour);
            MPI_Isend(&randomNumber, 1, MPI_INT, rightNeighbour, rightNeighbourTag, MPI_COMM_WORLD, &rightRequest);
        }
        if (topNeighbour > 0 &&
            currentRank != 1 && // this is the top side of the grid
            currentRank != 2 && // we dont want to send to our top neighbour 
            currentRank != 3 && // because we dont have a top neighbour
            currentRank != 4 &&
            currentRank != 5) {
            printf("rank: %d, top: %d\n\n", currentRank, topNeighbour);
            MPI_Isend(&randomNumber, 1, MPI_INT, topNeighbour, topNeighbourTag, MPI_COMM_WORLD, &topRequest);
        }
        if (bottomNeighbour < 21 &&
            currentRank != 16 && // this is the bottom side of the grid
            currentRank != 17 && // we dont want to send to our bottom neighbour 
            currentRank != 18 && // because we dont have a bottom neighbour
            currentRank != 19 &&
            currentRank != 20) {
            printf("rank: %d, bottom: %d\n\n", currentRank, bottomNeighbour);
            MPI_Isend(&randomNumber, 1, MPI_INT, bottomNeighbour, bottomNeighbourTag, MPI_COMM_WORLD, &bottomRequest);
        }
        
        // if corner dont receive
        // if edge, only receive 3
        if (currentRank != 1 && // top left corner
            currentRank != 5 && // top right corner
            currentRank != 16 && // bottom left corner
            currentRank != 20) { // bottom right corner
            if (edge(currentRank) == 1) {
                // receive 3
            } else {
                // receive 4
            }
        }
    }

    printf("rank: %d\n\n", currentRank);

    MPI_Finalize();
}