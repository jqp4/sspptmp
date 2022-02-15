all: papi-example example2

papi-example: papi-example.c
	gcc -Wall -Werror $< -lpapi -o $@

example2: example2.c
	gcc -Wall -Werror $< -lpapi -o $@

clean:
	rm -f papi-example example2
