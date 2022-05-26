CC=g++
CFlags=-g -Wall
BINS=server
OBJS=server.o mystack.o 

all: $(BINS) clienttest clienttest2 client forktest memory.o

memory.o:
	$(CC) $(CFlags) -o memory.o memory.cpp -c

forktest: forktest.o
	$(CC) $(CFlags) forktest.o -o forktest

client: client.o
	$(CC) $(CFlags) -o $@  $^ 

clienttest: clienttest.o
	$(CC) $(CFlags) -o $@  $^ -lpthread

clienttest2: clienttest2.o
	$(CC) $(CFlags) -o $@  $^ -lpthread


server: server.o mystack.o
	$(CC) $(CFlags) -o $@  server.o mystack.o

server.o:server.cpp 
	$(CC) $(CFlags) -c server.cpp 

mystack.o: mystack.cpp mystack.hpp
	$(CC) $(CFlags) -c mystack.cpp 

%: %.cpp
	$(CC) $(CFlags) -c -o $@  $^ -lpthread

clean:
	rm -f *.dSYM $(BINS) *.o client clienttest clienttest2 forktest