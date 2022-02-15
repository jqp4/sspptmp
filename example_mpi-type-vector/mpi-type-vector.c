#include <stdio.h>
#include <mpi.h>
#include <assert.h>

int
main(int argc, char **argv)
{
    enum { n = 10 };
    int rank, a[n], i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (i = 0; i < n; ++i) {
        if (!rank) {
            a[i] = i + 1;
        } else {
            a[i] = 0;
        }
    }
    
    if (!rank) {
        MPI_Datatype newtype;
        assert(MPI_Type_vector(n / 2, 1, 2, MPI_INT, &newtype) == MPI_SUCCESS);
        assert(MPI_Type_commit(&newtype) == MPI_SUCCESS);
        
        assert(MPI_Send(a, 1, newtype, 1, 0, MPI_COMM_WORLD) == MPI_SUCCESS);
        
        assert(MPI_Type_free(&newtype) == MPI_SUCCESS);
    } else {
        MPI_Recv(a, n / 2, MPI_INT, 0, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }

    printf("%d: %d", rank, a[0]);
    for (i = 1; i < n; ++i) {
        printf(" %d", a[i]);
    }
    putchar('\n');
    
    MPI_Finalize();
    return 0;
}
