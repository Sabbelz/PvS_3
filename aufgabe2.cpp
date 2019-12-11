#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define MASTER 0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int MatARow = atoi(argv[1]);
    int MatACol = atoi(argv[2]);
    int MatBCol = atoi(argv[3]);
    MPI_Status status;
    int numThreads, threadID;
    int row, rowsToBeSent, rest, offset;
    MPI_Comm_rank(MPI_COMM_WORLD, &threadID);
    MPI_Comm_size(MPI_COMM_WORLD, &numThreads);
    int numWorkers = numThreads - 1;

    float MatC[MatARow][MatBCol];   //result
    float MatA[MatARow][MatACol];
    float MatB[MatACol][MatBCol];

    if (numThreads < 2) {
        printf("You need at least 2 Threads  (Worker & Master)\n");
        exit(3);
    }
    if (threadID == MASTER) {

        /* Fill Matrices */
        for (int i = 0; i < MatARow; i++) {
            for (int j = 0; j < MatACol; j++) {
                MatA[i][j] = (float) (rand() % 10);
            }
        }

        for (int i = 0; i < MatACol; i++) {
            for (int j = 0; j < MatBCol; j++) {
                MatB[i][j] = (float) (rand() % 10);
            }
        }

        double start = MPI_Wtime();
        row = MatARow / numWorkers;
        rest = MatARow % numWorkers;
        offset = 0;

        // send to worker)

        for (int worker = 1; worker <= numWorkers; worker++) {
            if (worker <= rest) {
                rowsToBeSent = row + 1;
            } else {
                rowsToBeSent = row;
            }
            MPI_Send(&offset, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            MPI_Send(&rowsToBeSent, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            MPI_Send(&MatA[offset][0], rowsToBeSent * MatACol, MPI_FLOAT, worker, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&MatB, MatACol * MatBCol, MPI_FLOAT, worker, 0, MPI_COMM_WORLD);
            offset = offset + rowsToBeSent;
        }

        // recieve from workers
        for (int i = 1; i <= numWorkers; i++) {
            MPI_Recv(&offset, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&rowsToBeSent, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&MatC[offset][0], rowsToBeSent * MatBCol, MPI_FLOAT, i, 0,
                     MPI_COMM_WORLD, &status);
        }
        double end = MPI_Wtime() - start;
        float resultTest[MatARow][MatBCol];   //result of test

        // calculate matrix normally
        for (int i = 0; i < MatACol; i++) {
            for (int j = 0; j < MatBCol; j++) {
                resultTest[i][j] = 0;
                for (int k = 0; k < MatARow; k++)   //vllt hie MatBCol
                    resultTest[i][j] += MatA[i][k] * MatB[k][j];
            }
        }
        // Now Check if equal
        for (int i = 0; i < MatARow; i++) {
            for (int j = 0; j < MatBCol; j++) {
                    //printf("%f - %f\n" ,MatC[i][j], resultTest[i][j]);
                if (MatC[i][j] != resultTest[i][j]) {
                    printf("Matrix is wrong\n");
                    exit(4);
                }
            }
        }
        printf("Matrix multiplication was succesfully performed in %f s\n", end);
    }

    if (threadID != MASTER) {
        MPI_Recv(&offset, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&rowsToBeSent, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MatA, rowsToBeSent * MatACol, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&MatB, MatACol * MatBCol, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD, &status);
        for (int k = 0; k < MatBCol; k++)
            for (int i = 0; i < rowsToBeSent; i++) {
                MatC[i][k] = 0.0;
                for (int j = 0; j < MatACol; j++)
                    MatC[i][k] = MatC[i][k] + MatA[i][j] * MatB[j][k];
            }
        MPI_Send(&offset, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&rowsToBeSent, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
        MPI_Send(&MatC, rowsToBeSent * MatBCol, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}