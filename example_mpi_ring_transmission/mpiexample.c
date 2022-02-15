#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

void
shift(int rank, int np, int *x, int *y, int n)
{
    if (rank % 2) {
        MPI_Send(x, n, MPI_INT, (rank + 1) % np, 0,
                 MPI_COMM_WORLD);
        MPI_Recv(y, n, MPI_INT, (rank + np - 1) % np, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(y, n, MPI_INT, (rank + np - 1) % np, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(x, n, MPI_INT, (rank + 1) % np, 0,
                 MPI_COMM_WORLD);
    }
}

int
main(int argc, char **argv)
{
    int rank, np;
    int *x, *y;
    int n = 250*1000*1000;
    

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    x = malloc(n * sizeof(x[0]));
    y = malloc(n * sizeof(y[0]));
    assert(x != NULL && y != NULL);

    x[0] = rank + 1;
    y[0] = 0;
    
    printf("%d: x = %d\n", rank, x[0]);

    shift(rank, np, x, y, n);

    printf("%d: y = %d\n", rank, y[0]);

    free(x);
    free(y);
    
    MPI_Finalize();
    return 0;
}
