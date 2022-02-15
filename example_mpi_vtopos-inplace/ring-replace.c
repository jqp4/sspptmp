#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <assert.h>

void
shift(int *arr, int n, MPI_Comm comm, int source, int dest)
{
    MPI_Sendrecv_replace(arr, n, MPI_INT, dest, 0, source, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);
}


int
main(int argc, char **argv)
{
    enum {n = 100};
    int np, rank, period = 1, res, newrank, dest, source, buf[n];
    MPI_Comm comm_ring;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    res = MPI_Cart_create(MPI_COMM_WORLD, 1, &np, &period, 1, &comm_ring);
    assert(res == MPI_SUCCESS);
    MPI_Comm_rank(comm_ring, &newrank);
    MPI_Cart_shift(comm_ring, 0, 1, &source, &dest);
    buf[0] = newrank;
    shift(buf, n, comm_ring, source, dest);
    printf("%d: buf[0] = %d\n", newrank, buf[0]);


    MPI_Finalize();
    return 0;
}
