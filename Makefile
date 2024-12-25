opt=-O3 -Wall -std=c2x

all: clean macc2

macc2: macc2.o
	cc ${opt} $^ -o $@

macc2.o: macc2.c
	cc ${opt} -c $^

clean:
	rm -f macc2 *.o
