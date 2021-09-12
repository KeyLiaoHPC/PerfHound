CC = mpicc
CXX = mpicpc
CFLAGS = -O3 -DOSNOISE
LIBs = -lgsl -lopenblas

all: Parse variables pfhsim

Parse: Parse/*.c
	$(CC) -c Parse/*.c 

variables: variables.c
	$(CC) -c variables.c

pfhsim: pfhsim.c
	$(CC) -c pfhsim.c $(CFLAGS) $(LIBs)	
	$(CC) *.o -o pfhsim $(CFLAGS) $(LIBs)
clean:
	rm -rf *.o
	rm pfhsim

