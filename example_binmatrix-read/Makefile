CC = mpicc
CFLAGS = -Wall -Werror
LIBS =
PROGS = binmatrix-knife load-matrix-stdio load-matrix-mpiio

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)
