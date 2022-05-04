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
#define PORT 5001

int main()
{
    // node_stack_t *head_stack;
    // init_stack(head_stack);
    node_stack_t *head_stack = (node_stack_t *)mmap(NULL, sizeof(node_stack_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
     strcpy(head_stack->txt, "head");

    int filedescriptor_example;
    filedescriptor_example = open("1.txt", O_WRONLY);
    char buffer_ruby_test[BUFSIZE] = "hi from server ";
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    int *size = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *size = 0;
    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    char buffer[1024];
    char size_message[1024];
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", PORT);

    if (listen(sockfd, 100) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]Error in binding.\n");
    }

    while (1)
    {
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
        if (newSocket < 0)
        {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0)
        {
            close(sockfd);

            while (1)
            {
                recv(newSocket, buffer, 1024, 0);
                // Here, I initialize. Means, set this up for action.
                lock.l_type = F_WRLCK;
                // I There are locks available as write lock, read lock, refer man page please
                // Here, we try a write lock. F_RDLCK has to be used for read lock.
                fcntl(filedescriptor_example, F_SETLKW, &lock);
                if (strcmp(buffer, "exit") == 0)
                {
                    lock.l_type = F_UNLCK;
                    fcntl(filedescriptor_example, F_SETLKW, &lock);
                    close(newSocket);
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    return 0;
                }
                else if (strncmp(buffer, "print", 5) == 0)
                {
                    print_stack(head_stack);
                }
                else if (strncmp(buffer, "PUSH", 4) == 0)
                {
                    // printf("PUSH %c\n", buffer[5]);
                    // printf("DEBUG:from client : %s\n", buffer);

                    memcpy(buffer, buffer + 5, MAX_LIMIT - 5);
                    node_stack_t *new_node = (node_stack_t *)mmap(NULL, sizeof(node_stack_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
                    strcpy(new_node->txt, buffer);
                    new_node->next = head_stack;
                    head_stack = new_node;
                    printf("DEBUG:from server : %s\n", head_stack->txt);
                    // char *msg = push(buffer, head_stack, new_node);
                    // printf("DEBUG: afte pushed: %s\n", msg);
                    *size = *size + 1;
                    // printf("size in push: %d\n", *size);

                    // printf("server: pushed and newSocket is %d\n", *newSocket);
                    // sleep(5);
                    // printf("DEBUG: push good! socket %d\n", client_socket);
                }
                else if (strncmp(buffer, "POP", 3) == 0)
                {

                    // printf("DEBUG: from client : %s \n", buffer);
                    if (*size == 0)
                    {
                        // do noting
                    }
                    else
                    {
                        pop(head_stack, *size);
                        *size = *size - 1;
                    }

                    // printf("DEBUG: pop good!\n");
                }
                else if (strncmp(buffer, "TOP", 3) == 0)
                {
                    // printf("DEBUG: from client : %s \n", buffer);
                    if (*size == 0)
                    {
                        // do noting
                    }
                    else
                    {

                        char *msg = top(head_stack);
                        printf("DEBUG: TOP value %s\n", msg);
                        send(newSocket, msg, 1024, 0);
                        munmap(msg, strlen(msg));
                    }
                }
                else if (strncmp(buffer, "size", 4) == 0)
                {
                    // printf("DEBUG: IN SIZE: %s \n", buffer);
                    // printf("DEBUG: size call \n");
                    bzero(buffer, sizeof(buffer));
                    strcat(buffer, "DEBUG:");
                    sprintf(size_message, "%d", *size);
                    strncat(buffer, size_message, 1024);
                    printf("DEBUG: size call %d\n", *size);
                    send(newSocket, buffer, sizeof(buffer), 0);
                }
                if (strncmp(buffer, "hello from ruby \n", 17) == 0) /* hello from ruby \n */
                {

                    send(newSocket, buffer_ruby_test, 1024, 0);
                    bzero(buffer_ruby_test, sizeof(buffer_ruby_test));
                    sleep(1);
                    // unlock with fcntl
                    lock.l_type = F_UNLCK;
                    fcntl(filedescriptor_example, F_SETLKW, &lock);
                    close(newSocket);
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                    return 0;
                }
                lock.l_type = F_UNLCK;
                fcntl(filedescriptor_example, F_SETLKW, &lock);
            }
        }
    }

    close(newSocket);

    return 0;
}