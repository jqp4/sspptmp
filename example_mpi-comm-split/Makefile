CC = mpicc
CFLAGS = -Wall -Werror
LIBS =
PROGS = mpi-comm-split

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)
