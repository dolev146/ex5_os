#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "mystack.hpp"
// include mmap header
#include <sys/mman.h>
#define MAX_LIMIT 1024
#define BUFSIZE 1024
#define PORT 5004

char buffer_ruby_test[BUFSIZE] = "hi from server ";
char size_message[1024];
pid_t childpid = -1;

void kill_child(int sig)
{
    kill(childpid, SIGKILL);
}

int main()
{
    signal(SIGALRM, (void (*)(int))kill_child);
    // node_stack_t *head_stack;
    // init_stack(head_stack);
    node_stack_t *head_stack = (node_stack_t *)mmap(NULL, sizeof(node_stack_t) * 1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (head_stack == MAP_FAILED)
    {
        printf("Mapping Failed\n");
        return 1;
    }
    strcpy(head_stack->txt, "NULL");
    head_stack->next = head_stack + sizeof(node_stack_t);
    int *counter = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *counter = 0;

    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    int sockfd, ret;
    struct sockaddr_in serverAddr;
    int newSocket;
    struct sockaddr_in newAddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("DEBUG:[-]Error in connection.\n");
        exit(1);
    }
    printf("DEBUG:[+]Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); // bind the socket to the server address
    if (ret < 0)
    {
        printf("DEBUG:[-]Error in binding.\n");
        exit(1);
    }
    printf("DEBUG:[+]Bind to port %d\n", PORT);

    if (listen(sockfd, 100) == 0)
    {
        printf("DEBUG:[+]Listening....\n");
    }
    else
    {
        printf("DEBUG:[-]Error in binding.\n");
    }

    while (1)
    {
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size); // accept the connection
        if (newSocket < 0)
        {
            exit(1);
        }
        printf("DEBUG:Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0)
        {
            char buffer[1024];
            int filedescriptor_example = open("1.txt", O_WRONLY);
            while (1)
            {
                if (!recv(newSocket, buffer, 1024, 0))
                {
                    printf("DEBUG:[-]Connection closed.\n");
                    exit(1);
                }
                lock.l_type = F_WRLCK;
                fcntl(filedescriptor_example, F_SETLKW, &lock);
                printf("DEBUG:lock \n");
                if (strncmp(buffer, "PUSH", 4) == 0)
                {
                    // printf("DEBUG:PUSH\n");
                    memcpy(buffer, buffer + 5, MAX_LIMIT - 5);
                    strcpy(head_stack[*counter].txt, buffer);
                    (*counter)++;
                }
                if (strncmp(buffer, "TOP", 3) == 0)
                {
                    // printf("DEBUG:TOP\n");
                    char * msg = head_stack[*counter - 1].txt;
                    // concat "OUTPUT" and msg
                    char * msg_output = (char *)malloc(sizeof(char) * (strlen(msg) + strlen("OUTPUT") + 1));
                    strcpy(msg_output, "OUTPUT:");
                    strcat(msg_output, msg);
                    // send msg_output
                    send(newSocket, msg_output, strlen(msg_output), 0);
                    // send(newSocket, head_stack[*counter - 1].txt, 1024, 0);
                }
                if (strncmp(buffer, "POP", 3) == 0)
                {
                    // printf("DEBUG:POP\n");
                    strcpy(head_stack[*counter].txt, "");
                    (*counter)--;
                }
                if (strncmp(buffer, "exit", 4) == 0)
                {
                    bzero(buffer, 1024);
                    lock.l_type = F_UNLCK;
                    fcntl(filedescriptor_example, F_SETLKW, &lock);
                    printf("DEBUG:EXIT\n");
                    return 0;
                }
                if (strncmp(buffer, "size", 4) == 0)
                {
                    sprintf(size_message, "%d", *counter);
                    send(newSocket, size_message, 1024, 0);
                }
                if (strncmp(buffer, "ruby", 4) == 0)
                {
                    // printf("DEBUG:ruby\n");
                    send(newSocket, buffer_ruby_test, 1024, 0);
                }

                bzero(buffer, 1024);
                lock.l_type = F_UNLCK;
                fcntl(filedescriptor_example, F_SETLKW, &lock);
                printf("DEBUG:unlock \n");
            }
        }
    }
    wait(NULL);
    close(newSocket);

    return 0;
}