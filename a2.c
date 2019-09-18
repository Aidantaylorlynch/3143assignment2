#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int numberOfProcesses;
    int currentRank;
    int masterRank = 0;
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
        // we dont want outside edges to send to neighbours outside the bounds
        if (leftNeighbour > 0 &&
            currentRank != 1 && // this is the left side of the grid
            currentRank != 5 && // we dont want to send to our left neighbour 
            currentRank != 11 && // because we dont have a left neighbour
            currentRank != 16) {
            printf("rank: %d, left: %d\n\n", currentRank, leftNeighbour);
        }
        if (rightNeighbour < 21 &&
            currentRank != 5 && // this is the right side of the grid
            currentRank != 10 && // we dont want to send to our right neighbour 
            currentRank != 15 && // because we dont have a right neighbour
            currentRank != 20) {
            printf("rank: %d, right: %d\n\n", currentRank, rightNeighbour);
        }
        if (topNeighbour > 0 &&
            currentRank != 1 && // this is the top side of the grid
            currentRank != 2 && // we dont want to send to our top neighbour 
            currentRank != 3 && // because we dont have a top neighbour
            currentRank != 4 &&
            currentRank != 5) {
            printf("rank: %d, top: %d\n\n", currentRank, topNeighbour);
        }
        if (bottomNeighbour < 21 &&
            currentRank != 16 && // this is the bottom side of the grid
            currentRank != 17 && // we dont want to send to our bottom neighbour 
            currentRank != 18 && // because we dont have a bottom neighbour
            currentRank != 19 &&
            currentRank != 20) {
            printf("rank: %d, bottom: %d\n\n", currentRank, bottomNeighbour);
        }
        
    }

    printf("rank: %d\n\n", currentRank);

    MPI_Finalize();
}