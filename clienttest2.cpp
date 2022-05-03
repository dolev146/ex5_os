#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
// #include <stdlib.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <assert.h>

void *cientThread(void *arg)
{
    printf("In thread\n");
    char message[1000];

    char buffer[1024];
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // Create the socket.
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address
    // Address family is Internet
    serverAddr.sin_family = AF_INET;

    // Set port number, using htons function
    serverAddr.sin_port = htons(5000);

    // Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);

    strcpy(message, "hello from ruby \n");
    if (send(clientSocket, message, strlen(message), 0) < 0)
    {
        printf("Send failed\n");
    }
    usleep(5000);
    recv(clientSocket, message, strlen(message), 0);
    printf("%s\n", message);

    close(clientSocket);
    pthread_exit(NULL);
}

int main()
{
    pthread_t a;
    pthread_create(&a, NULL, cientThread, NULL);
    pthread_t b;
    pthread_create(&b, NULL, cientThread, NULL);
    pthread_t c;
    pthread_create(&c, NULL, cientThread, NULL);
    pthread_t d;
    pthread_create(&d, NULL, cientThread, NULL);

    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);
    pthread_join(d, NULL);

    return 0;
}