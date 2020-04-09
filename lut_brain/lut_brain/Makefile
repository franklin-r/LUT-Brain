CC = g++
CFLAGS = -Wall -g --std=c++11
OBJECTS = main.o Image.o NN.o NNLayer.o asm.o

all : main

main : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main

%.o : %.cc
	$(CC) $(CFLAGS) -c $<

clean :
	rm -rf *.o main

.PHONY: all clean

