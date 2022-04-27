CC=g++
CFlags=-g -Wall
BINS=server
OBJS=server.o myqueue.o mystack.o mymemory.o

all: $(BINS) clienttest client

client: client.o
	$(CC) $(CFlags) -o $@  $^ -lpthread

clienttest: clienttest.o
	$(CC) $(CFlags) -o $@  $^ -lpthread


server: $(OBJS)
	$(CC) $(CFlags) -o $@  $^ -lpthread

%: %.cpp
	$(CC) $(CFlags) -c -o $@  $^ -lpthread

clean:
	rm -f *.dSYM $(BINS) *.o client clienttest