#include <stdio.h>
#include <mpi.h>
#include <assert.h>

void
shift(int *x, int *y, int n, MPI_Comm comm, int source, int dest)
{
    MPI_Request rqs[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};

    MPI_Isend(x, n, MPI_INT, dest, 0, comm, &rqs[0]);
    MPI_Irecv(y, n, MPI_INT, source, 0, comm, &rqs[1]);

    MPI_Waitall(2, rqs, MPI_STATUS_IGNORE);
}

int
main(int argc, char **argv)
{
    enum {n = 100};
    int np, rank, period = 1, res, newrank, dest, source, x[n], y[n];
    MPI_Comm comm_ring;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    res = MPI_Cart_create(MPI_COMM_WORLD, 1, &np, &period, 1, &comm_ring);
    assert(res == MPI_SUCCESS);
    MPI_Comm_rank(comm_ring, &newrank);
    MPI_Cart_shift(comm_ring, 0, 1, &source, &dest);
    x[0] = newrank;
    shift(x, y, n, comm_ring, source, dest);
    printf("%d: y[0] = %d\n", newrank, y[0]);


    MPI_Finalize();
    return 0;
}
