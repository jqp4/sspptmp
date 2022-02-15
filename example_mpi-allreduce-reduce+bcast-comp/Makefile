CC = mpicc
CFLAGS = -Wall -Werror
LIBS =
PROGS = mpi-allreduce-reduce+bcast-comp

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)
