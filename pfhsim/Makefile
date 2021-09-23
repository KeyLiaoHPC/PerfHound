CC=mpicc

all:
	$(CC) -c *.c -O3
	$(CC) *.o -o pfhsim -lgsl -lopenblas -O3
clean:
	rm *.o
	rm pfhsim
