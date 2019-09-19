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

int eventOccurred(int eventBuffer[4][2]) {
    if (eventBuffer[3][0] == 0) { // if 0, then this is an event from an edge node, and we need to compare all 3 events
        if (eventBuffer[0][0] == eventBuffer[1][0] && eventBuffer[1][0] == eventBuffer[2][0]) {
            // event occurred
            return 1;
        } else {
            // event did not occur
            return 0;
        }
    } else { // this is an event from a non-edge node so we need to check all 4 events
        // sort array, so we only have to compare the first and last 3 elements to detect an event
        int i;
        int j;
        for (i = 0; i < 4; i ++) {
            for (j = i + 1; j < 4; j ++) {
                if (eventBuffer[i][0] > eventBuffer[j][0]) {
                    int temp[2];
                    temp[0] = eventBuffer[i][0];
                    temp[1] = eventBuffer[i][1];
                    eventBuffer[i][0] = eventBuffer[j][0];
                    eventBuffer[i][1] = eventBuffer[j][1];
                    eventBuffer[j][0] = temp[0];
                    eventBuffer[j][1] = temp[1];
                }
            }
        }
        if (eventBuffer[0][0] == eventBuffer[1][0] && eventBuffer[1][0] == eventBuffer[2][0] || // compare first 3 elements
            eventBuffer[1][0] == eventBuffer[2][0] && eventBuffer[2][0] == eventBuffer[3][0] ) { // compare last 3 elements
            return 1;
        } else {
            return 0;
        }
    }
}

int main(int argc, char **argv) {
    FILE *fp;
    int numberOfProcesses;
    int currentRank;
    int masterRank = 0;
    int leftNeighbourTag = 0;
    int rightNeighbourTag = 1;
    int topNeighbourTag = 2;
    int bottomNeighbourTag = 3;
    int messageWaiting;
    double maxTimeInterval = 10;
    double iterationElapsedTime;
    double iterationStartTime;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    iterationStartTime = MPI_Wtime();
    iterationElapsedTime = 0;
    if (currentRank == masterRank) {
        fp = fopen("log.txt", "w");
        int eventCounter = 0;
        //receive for a certain time period
        while (iterationElapsedTime < maxTimeInterval) {
            MPI_Status status;
            messageWaiting = 0;
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &messageWaiting, &status);
            if (messageWaiting) {
                // receive
                int buff[4][2];
                MPI_Status status;
                MPI_Recv(&buff, 8, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                eventCounter += 1;
                // entries in buffer that are 0, are non events
                // non-zero entries are the ranks of the node that triggered the event
		        fprintf(fp, "Event received from node %d (reference node)\n", status.MPI_SOURCE);
                fprintf(fp, "Details: \n");
                fprintf(fp, "Node: %d, Event Value: %d (adjacent node)\n", buff[0][1], buff[0][0]);
                fprintf(fp, "Node: %d, Event Value: %d (adjacent node)\n", buff[1][1], buff[1][0]);
                fprintf(fp, "Node: %d, Event Value: %d (adjacent node)\n", buff[2][1], buff[2][0]);
                if (buff[3][1] != 0) { // check if event was triggered by 3 or 4 nodes
                    fprintf(fp, "Node: %d, Event Value: %d (adjacent node)\n", buff[3][1], buff[3][0]);
                }
                fprintf(fp, "\n");
            }
            iterationElapsedTime = MPI_Wtime() - iterationStartTime; // update time
            printf("loading: %.0f%%\r", (iterationElapsedTime/maxTimeInterval * 100));
        }
        fprintf(fp, "Total number of events that occurred: %d\n", eventCounter);

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
        int event[2];
        event[0] = randomNumber;
        event[1] = currentRank;
        // we dont want outside edges to send to neighbours outside the bounds
        if (leftNeighbour > 0 &&
            currentRank != 1 && // this is the left side of the grid
            currentRank != 6 && // we dont want to send to our left neighbour 
            currentRank != 11 && // because we dont have a left neighbour
            currentRank != 16) {
            // printf("rank: %d, left: %d\n\n", currentRank, leftNeighbour);
            MPI_Isend(&event, 2, MPI_INT, leftNeighbour, leftNeighbourTag, MPI_COMM_WORLD, &leftRequest);
        }
        if (rightNeighbour < 21 &&
            currentRank != 5 && // this is the right side of the grid
            currentRank != 10 && // we dont want to send to our right neighbour 
            currentRank != 15 && // because we dont have a right neighbour
            currentRank != 20) {
            // printf("rank: %d, right: %d\n\n", currentRank, rightNeighbour);
            MPI_Isend(&event, 2, MPI_INT, rightNeighbour, rightNeighbourTag, MPI_COMM_WORLD, &rightRequest);
        }
        if (topNeighbour > 0 &&
            currentRank != 1 && // this is the top side of the grid
            currentRank != 2 && // we dont want to send to our top neighbour 
            currentRank != 3 && // because we dont have a top neighbour
            currentRank != 4 &&
            currentRank != 5) {
            // printf("rank: %d, top: %d\n\n", currentRank, topNeighbour);
            MPI_Isend(&event, 2, MPI_INT, topNeighbour, topNeighbourTag, MPI_COMM_WORLD, &topRequest);
        }
        if (bottomNeighbour < 21 &&
            currentRank != 16 && // this is the bottom side of the grid
            currentRank != 17 && // we dont want to send to our bottom neighbour 
            currentRank != 18 && // because we dont have a bottom neighbour
            currentRank != 19 &&
            currentRank != 20) {
            // printf("rank: %d, bottom: %d\n\n", currentRank, bottomNeighbour);
            MPI_Isend(&event, 2, MPI_INT, bottomNeighbour, bottomNeighbourTag, MPI_COMM_WORLD, &bottomRequest);
        }
        
        // if corner dont receive
        // if edge, only receive 3
        int loopCounter;
        int eventCounter = 0;
        int buff[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
        if (currentRank != 1 && // top left corner
            currentRank != 5 && // top right corner
            currentRank != 16 && // bottom left corner
            currentRank != 20) { // bottom right corner
            if (edge(currentRank) == 1) {
                // receive 3
                for (loopCounter = 0; loopCounter < 3; loopCounter ++) {
                    MPI_Status status;
                    MPI_Recv(&buff[loopCounter][0], 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    // printf("rank: %d, received %d from %d, which is this processes %d\n", currentRank, buff[loopCounter] ,status.MPI_SOURCE, status.MPI_TAG);
                }

            } else {
                // receive 4
                for (loopCounter = 0; loopCounter < 4; loopCounter ++) {
                    MPI_Status status;
                    MPI_Recv(&buff[loopCounter][0], 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    // printf("rank: %d, received %d from %d, which is this processes %d\n", currentRank, buff[loopCounter] ,status.MPI_SOURCE, status.MPI_TAG);
                }
            }
            // buffer is filled with events if they occured
            if (eventOccurred(buff)) {
                MPI_Send(&buff, 8, MPI_INT, masterRank, 1, MPI_COMM_WORLD);
            }
            // if (buff[0][0] == 1) {
            //     eventCounter += 1;
            // }
            // if (buff[1][0] == 1) {
            //     eventCounter += 1;
            // }
            // if (buff[2][0] == 1) {
            //     eventCounter += 1;
            // }
            // if (buff[3][0] == 1) { // for processes that only receive 3 events, this defaults to 0 and wont affect results
            //     eventCounter += 1;
            // }
            // printf("rank: %d, events: %d\n", currentRank, eventCounter);
        }
    }


    MPI_Finalize();
}