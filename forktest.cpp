#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

int main(void)
{
    // int pfds[2];
    // pipe(pfds);

    int pid1 = fork();
    if (pid1 == 0)
    {
        // pid 1
        char buf[1024];

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
        serverAddr.sin_port = htons(5004);

        // Set IP address to localhost
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        // Connect the socket to the server using the address
        addr_size = sizeof serverAddr;
        connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);
        strcpy(message, "PUSH a");
        for (int i = 0; i < 100; i++)
        {
            send(clientSocket, message, strlen(message), 0);
            usleep(5000);
        }
        close(clientSocket);
    }
    else
    {
        // parent
        // pid 1
        char buf[1024];

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
        serverAddr.sin_port = htons(5004);

        // Set IP address to localhost
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        // Connect the socket to the server using the address
        addr_size = sizeof serverAddr;
        connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);
        strcpy(message, "PUSH a");
        for (int i = 0; i < 100; i++)
        {
            send(clientSocket, message, strlen(message), 0);
            usleep(5000);
        }

        wait(NULL);
        send(clientSocket, "size", 4, 0);
        recv(clientSocket, buffer, 1024, 0);
        printf("PARENT size %s\n", buffer);
        
        assert(buffer[0] == '2');
        assert(buffer[1] == '0');
        assert(buffer[2] == '0');
    }
    return 0;
}