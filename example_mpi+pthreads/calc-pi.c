#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

struct thr_in
{
    int rank, np, nt, id;
    int N;
};

int
min(int a, int b)
{
    return a > b ? b : a;
}

void *
job(void *data0)
{
    struct thr_in *in = (struct thr_in *) data0;

    int rank, np, nt, id;
    int N;

    int njobs, glob_idx;
    int n, ib, ie, i;

    double h, local_sum = 0.0;
    double *res;
    
    rank = in->rank;
    np = in->np;
    nt = in->nt;
    id = in->id;
    N = in->N;

    njobs = np * nt;
    glob_idx = nt * rank + id;
    
    n = N / njobs + (glob_idx < N % njobs);
    ib = N / njobs * glob_idx + min(glob_idx, N % njobs);
    ie = ib + n;
    
    h = 1.0 / N;

    for (i = ib; i < ie; ++i) {
        double x = i * h + 0.5 * h;

        local_sum += 4.0 / (1.0 + x * x) * h;
    }

    res = calloc(1, sizeof(*res));
    *res = local_sum;

    return res;
}

int
main(int argc, char **argv)
{
    int rank, np, nthreads, N, i, res;
    double start, finish, dt, dt_glob, pi_local = 0.0, pi;
    
    struct thr_in *ctxs = NULL;
    pthread_t *thrds = NULL;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) {
        assert(argc == 3);
        N = atoi(argv[1]);
        nthreads = atoi(argv[2]);
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nthreads, 1, MPI_INT, 0, MPI_COMM_WORLD);

    ctxs = calloc(nthreads, sizeof(ctxs[0]));
    thrds = calloc(nthreads, sizeof(thrds[0]));
    assert(ctxs != NULL && thrds != NULL);

    start = MPI_Wtime();
    for (i = 0; i < nthreads; ++i) {
        ctxs[i].rank = rank;
        ctxs[i].np = np;
        ctxs[i].nt = nthreads;
        ctxs[i].id = i;
        ctxs[i].N = N;
        res = pthread_create(&thrds[i], NULL, job, (void *) &ctxs[i]);
        assert(res == 0);
    }

    for (i = 0; i < nthreads; ++i) {
        double *local_sum;
        res = pthread_join(thrds[i], (void **) &local_sum);
        assert(res == 0);

        pi_local += *local_sum;

        free(local_sum);
    }
    finish = MPI_Wtime();

    dt = finish - start;
    MPI_Reduce(&dt, &dt_glob, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&pi_local, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("pi = %.12f\n", pi);
        printf("elapsed time = %f s\n", dt_glob);
    }

    free(ctxs);
    free(thrds);
    
    MPI_Finalize();
}
