CC = mpicc
CFLAGS = -Wall -Werror
LIBS =
PROGS = mpi-type-vector

all: $(PROGS)

% : %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)
