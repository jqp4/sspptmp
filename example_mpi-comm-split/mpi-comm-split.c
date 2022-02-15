#include <stdio.h>
#include <mpi.h>
#include <assert.h>


int
main(int argc, char **argv)
{
    int rank, dims[2], np, periods[2], coords[2], rowsize, rowrank;
    MPI_Comm cart, myrow;

    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    assert(np == 9);

    dims[0] = 3;
    dims[1] = 3;
    periods[0] = 0;
    periods[1] = 0;

    
    assert(MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart) ==
           MPI_SUCCESS);
    MPI_Comm_rank(cart, &rank);
    assert(MPI_Cart_coords(cart, rank, 2, coords) == MPI_SUCCESS);
    MPI_Comm_split(cart, coords[0], coords[1], &myrow);

    MPI_Comm_size(myrow, &rowsize);
    MPI_Comm_rank(myrow, &rowrank);

    printf("%d: (%d, %d) (%d / %d)\n", rank, coords[0], coords[1],
           rowrank, rowsize);

    MPI_Comm_free(&myrow);
    
    MPI_Finalize();
    return 0;
}
