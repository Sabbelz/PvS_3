#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define MAT_A 0
#define MAT_B 1
#define MAT_C 2

float **alloc_mat(int row, int col)
{
    float **A1, *A2;

    A1 = (float **)calloc(row, sizeof(float *));
    A2 = (float *)calloc(row*col, sizeof(float));
    for (int i = 0; i < row; i++)
        A1[i] = A2 + i*col;

    return A1;
}

void init_mat(float **A, int row, int col)
{
    for (int i = 0; i < row*col; i++)
        A[0][i] = (float)(rand() % 10);
}

void print_mat(float **A, int row, int col, char *tag)
{
    int i, j;

    printf("Matrix %s:\n", tag);
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
            printf("%6.1f   ", A[i][j]);
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    float **A, **B, **C;
    int d1, d2, d3;
    int i, j, k;

    int rank, size;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 4)
    {
        printf ("Matrix multiplication: C = A x B\n");
        printf ("Usage: %s <NumRowA> <NumColA> <NumColB>\n", argv[0]);
        return 0;
    }

    d1 = atoi(argv[1]);
    d2 = atoi(argv[2]);
    d3 = atoi(argv[3]);
    printf("Matrix sizes C[%d][%d] = A[%d][%d] x B[%d][%d]\n", d1, d3, d1, d2, d2, d3);

    A = alloc_mat(d1, d2);
    init_mat(A, d1, d2);
    B = alloc_mat(d2, d3);
    init_mat(B, d2, d3);
    C = alloc_mat(d1, d3);

    printf("Perform matrix multiplication...\n");

    if (rank == 0){
        int worker = size - 1;
        int row = d1 / worker;
        int rest = d1 % worker;
        int start = 0;

        for(int i = 0; i < worker; ++i){
            int srow = 0;
            if(i == worker){
                srow = rest + row;
            }

            MPI_Send(srow, 1, MPI_INT, i+1, MAT_C, MPI_COMM_WORLD);
            MPI_Send(A[start][0], srow*d1, MPI_INT, i+1, MAT_A, MPI_COMM_WORLD);
            MPI_Send(B, srow * d1, MPI_INT, i+1, MAT_B, MPI_COMM_WORLD);

        }
        

    }

    for (i = 0; i < d1; i++)
        for (j = 0; j < d3; j++)
            for (k = 0; k < d2; k++)
                C[i][j] += A[i][k] * B[k][j];

    print_mat(A, d1, d2, "A");
    print_mat(B, d2, d3, "B");
    print_mat(C, d1, d3, "C");

    printf ("\nDone.\n");

    MPI_Finalize();
    return 0;
}
