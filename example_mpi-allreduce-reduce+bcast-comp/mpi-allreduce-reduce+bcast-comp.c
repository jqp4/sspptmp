#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 MPI_Reduce();
 MPI_Bcast();
 */

void
my_allreduce(const void *sendbuf,
             void *recvbuf,
             int count,
             MPI_Datatype datatype,
             MPI_Op op,
             MPI_Comm comm)
{
    MPI_Reduce(sendbuf, recvbuf, count, datatype, op, 0, comm);
    MPI_Bcast(recvbuf, count, datatype, 0, comm);
}

int
main(int argc, char **argv)
{
    int n = 1024 * 1024, *in_buf = NULL, *out_buf = NULL, retval = 1;
    int i, rank, nit = 100;
    double t1, t2, t3, dt1, dt2, dt1max, dt2max;

    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    in_buf = malloc(n * sizeof(in_buf[0]));
    out_buf = malloc(n * sizeof(out_buf[0]));

    if (!in_buf || !out_buf) {
        fputs("memory allocation error\n", stderr);
        goto cleanup;
    }

    for (i = 0; i < n; ++i) {
        in_buf[i] = i + 1;
    }

    t1 = MPI_Wtime();
    for (i = 0; i < nit; ++i) {
        my_allreduce(in_buf, out_buf, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }
    t2 = MPI_Wtime();
    for (i = 0; i < nit; ++i) {
        MPI_Allreduce(in_buf, out_buf, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }
    t3 = MPI_Wtime();

    dt1 = t2 - t1;
    dt2 = t3 - t2;

    MPI_Reduce(&dt1, &dt1max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&dt2, &dt2max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("t1 = %f s\n"
               "t2 = %f s\n",
               dt1max,
               dt2max);
    }
    
    retval = 0;
 cleanup:
    if (in_buf) {
        free(in_buf);
    }
    if (out_buf) {
        free(out_buf);
    }
    MPI_Finalize();
    return retval;
}
