#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>

enum { ndims = 2 };

int *
read_block(MPI_File f,
           int msize,
           int block_size_i,
           int block_size_j,
           int offset_i,
           int offset_j)
{
    int *buf = NULL;
    MPI_Datatype ft;

    buf = malloc(block_size_i * block_size_j * sizeof(buf[0]));
    assert(buf != NULL);

    MPI_Type_vector(block_size_i, block_size_j, msize, MPI_INT, &ft);
    MPI_Type_commit(&ft);

    MPI_File_set_view(f, (1 + offset_i * msize + offset_j) * sizeof(int),
                      MPI_INT, ft, "native", MPI_INFO_NULL);

    MPI_File_read(f, buf, block_size_i * block_size_j, MPI_INT,
                  MPI_STATUS_IGNORE);

    return buf;
}

static inline int
min(int a, int b)
{
    return a > b ? b : a;
}

void
block_split(int globsize,
            int nchunks,
            int rank,
            int *locsize,
            int *globstart)
{
    int div = globsize / nchunks;
    int r = globsize % nchunks;

    *locsize = div + (rank < r);
    *globstart = div * rank + min(rank, r);
}

int *
load_matrix(const char *filename,
            MPI_Comm comm_2d,
            int *block_size_i,
            int *block_size_j,
            int *msize)
{
    int dims[ndims], periods[ndims], coords[ndims];
    int offset_i, offset_j;
    int *buf;
    MPI_File f;

    MPI_File_open(comm_2d, filename, MPI_MODE_RDONLY,
                  MPI_INFO_NULL, &f);

    MPI_File_read(f, msize, 1, MPI_INT, MPI_STATUS_IGNORE);

    MPI_Cart_get(comm_2d, ndims, dims, periods, coords);

    block_split(*msize, dims[0], coords[0], block_size_i, &offset_i);
    block_split(*msize, dims[1], coords[1], block_size_j, &offset_j);

    buf = read_block(f, *msize, *block_size_i, *block_size_j,
                     offset_i, offset_j);
    
    MPI_File_close(&f);
    return buf;
}

void
debug_print_block(int *block,
                  int block_size_i,
                  int block_size_j,
                  MPI_Comm comm_2d)
{
    enum { maxlen = 100 };
    char filename[maxlen];
    int dims[ndims], periods[ndims], coords[ndims];
    FILE *f = NULL;
    int i, j;
    MPI_Cart_get(comm_2d, ndims, dims, periods, coords);

    snprintf(filename, maxlen, "%d-%d.out", coords[0], coords[1]);
    f = fopen(filename, "w");
    assert(f != NULL);

    fprintf(f, "(%d, %d), shape = (%d, %d)\n", coords[0], coords[1],
            block_size_i, block_size_j);

    for (i = 0; i < block_size_i; ++i) {
        if (block_size_j) {
            fprintf(f, "    %d", block[i * block_size_j]);
            for (j = 1; j < block_size_j; ++j) {
                fprintf(f, " %d", block[i * block_size_j + j]);
            }
            fputc('\n', f);
        }
    }

    fputc('\n', f);
    
    fclose(f);
}


int
main(int argc, char **argv)
{
    int npx, npy, np;
    int dims[ndims], periods[ndims];
    MPI_Comm comm_2d;
    int *matrix_block, block_size_i, block_size_j, msize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    assert(argc == 4);
    npx = atoi(argv[1]);
    npy = atoi(argv[2]);

    assert(np == npx * npy);

    dims[0] = npx;
    dims[1] = npy;
    periods[0] = 0;
    periods[1] = 0;

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 1, &comm_2d);
    matrix_block = load_matrix(argv[3], comm_2d, &block_size_i, &block_size_j,
                               &msize);
    debug_print_block(matrix_block, block_size_i, block_size_j, comm_2d);
    
    free(matrix_block);
    MPI_Finalize();
    return 0;
}
