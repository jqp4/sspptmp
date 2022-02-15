all: calc-pi mpi-pthread-test

calc-pi: calc-pi.c
	mpicc -Wall -Werror $< -lm -pthread -o $@

mpi-pthread-test: mpi-pthread-test.c
	mpicc -Wall -Werror -pthread $< -o $@

clean:
	rm -f calc-pi mpi-pthread-test
