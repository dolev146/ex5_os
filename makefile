CC=g++
CFlags=-g -Wall
BINS=server
OBJS=server.o mystack.o

all: $(BINS) clienttest client

client: client.o
	$(CC) $(CFlags) -o $@  $^ 

clienttest: clienttest.o
	$(CC) $(CFlags) -o $@  $^ -lpthread


server: $(OBJS)
	$(CC) $(CFlags) -o $@  $^ 

%: %.cpp
	$(CC) $(CFlags) -c -o $@  $^ -lpthread

clean:
	rm -f *.dSYM $(BINS) *.o client clienttest