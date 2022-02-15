#include <stdio.h>
#include <mpi.h>


int
main(int argc, char **argv)
{
    enum { n = 100 };
    int buf[n], rank;

    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    buf[0] = 1;

    /* case "result only at root"

    if (rank == 0) {
        MPI_Reduce(MPI_IN_PLACE, buf, n, MPI_INT, MPI_SUM, 0,
                   MPI_COMM_WORLD);
    } else {
        MPI_Reduce(buf, NULL, n, MPI_INT, MPI_SUM, 0,
                   MPI_COMM_WORLD);
    }
    */

    MPI_Allreduce(MPI_IN_PLACE, buf, n, MPI_INT, MPI_SUM,
                  MPI_COMM_WORLD);
    
    printf("%d: buf[0] = %d\n", rank, buf[0]);
    
    MPI_Finalize();
}
