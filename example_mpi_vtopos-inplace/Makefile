CC = mpicc
CFLAGS = -Wall -Werror
PROGS = vtopos reduce-inplace ring-replace

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(PROGS)
