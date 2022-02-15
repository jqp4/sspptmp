#include <stdio.h>
#include <mpi.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

struct ctx
{
    int rank, np;
    int id, nt;
};


void *
job(void *arg)
{
    struct ctx *data = (struct ctx *) arg;

    if (data->np == 2 && data->nt == 2) {
        if (data->rank == 0) {
            int x = data->id;

            MPI_Send(&x, 1, MPI_INT, 1, data->id, MPI_COMM_WORLD);
        } else {
            int y;

            MPI_Recv(&y, 1, MPI_INT, 0, data->id, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("thread %d: got %d\n", data->id, y);
        }
    }

    return NULL;
}


int
main(int argc, char **argv)
{
    enum { max_threads = 64 };

    int i, rank, np, nt;
    int provided_mode;
    
    struct ctx ctxs[max_threads];
    pthread_t thrds[max_threads];

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided_mode);

    assert(provided_mode == MPI_THREAD_MULTIPLE);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    assert(argc == 2);

    nt = atoi(argv[1]);

    assert(nt <= max_threads);

    for (i = 0; i < nt; ++i) {
        ctxs[i].np = np;
        ctxs[i].rank = rank;
        ctxs[i].id = i;
        ctxs[i].nt = nt;

        assert(pthread_create(thrds + i, NULL, job, ctxs + i) == 0);
    }

    for (i = 0; i < nt; ++i) {
        assert(pthread_join(thrds[i], NULL) == 0);
    }

    MPI_Finalize();
    return 0;
}
