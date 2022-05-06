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
    strcpy(head_stack->txt, "NULL");
    head_stack->next = head_stack + sizeof(node_stack_t);

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

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
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
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
        if (newSocket < 0)
        {
            exit(1);
        }
        printf("DEBUG:Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

        if ((childpid = fork()) == 0)
        {
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
                    printf("DEBUG:PUSH\n");
                    memcpy(buffer, buffer + 5, MAX_LIMIT - 5);
                    node_stack_t *temp = head_stack + (sizeof(node_stack_t) * (*size));
                    strcpy(temp->txt, buffer);
                    // (*size)++
                }
                if (strncmp(buffer, "TOP", 3) == 0)
                {
                    printf("DEBUG:TOP\n");
                    node_stack_t *temp = head_stack + sizeof(node_stack_t) * (*size);
                    send(newSocket, temp->txt, 1024, 0);
                }
                if (strncmp(buffer, "POP", 3) == 0)
                {
                    printf("DEBUG:POP\n");
                    node_stack_t *temp = head_stack + sizeof(node_stack_t) * (*size);
                    strcpy(temp->txt, "");
                    (*size)--;
                }
                bzero(buffer, 1024);
                lock.l_type = F_UNLCK;
                fcntl(filedescriptor_example, F_SETLKW, &lock);
                printf("DEBUG:unlock \n");
            }
            // if (strncmp(buffer, "exit", 4) == 0)
            // {
            //     lock.l_type = F_UNLCK;
            //     fcntl(filedescriptor_example, F_SETLKW, &lock);
            //     close(newSocket);
            //     printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            //     return 0;
            // }

            // else if (strncmp(buffer, "PUSH", 4) == 0)
            // {
            //     memcpy(buffer, buffer + 5, MAX_LIMIT - 5);
            //     printf("heasdasdasdre5");
            //     if ((*size) == 0)
            //     {
            //         strcpy(head_stack->txt, buffer);
            //         (*size)++;
            //     }
            //     else if ((*size) <= 1000)
            //     {
            //         // catch the block of memory for the new node that is next to the head_stack
            //         // printf("PUSH call : %d , socket : %d\n", *size, newSocket);
            //         // int a = *size + 1;
            //         // printf("a : %d\n", a);
            //         // *size = a;
            //         printf("here5");
            //         (*size)++;
            //         printf("here4");
            //         node_stack_t *new_node = head_stack + (sizeof(node_stack_t) * (*size));
            //         // copy the head stack to the new node
            //         printf("here3");
            //         strcpy(new_node->txt, head_stack->txt);
            //         printf("here2");
            //         strcpy(head_stack->txt, buffer);
            //         printf("here");
            //     }
            //     else
            //     {
            //         printf("Stack is full.\n");
            //     }
            //     bzero(buffer, 1024);
            //     lock.l_type = F_UNLCK;
            //     fcntl(filedescriptor_example, F_SETLKW, &lock);
            //     printf("unlock \n");
            //     return 0;
            // }
            // else if (strncmp(buffer, "TOP", 3) == 0)
            // {
            //     printf("DEBUG: TOP ");
            //     // printf("DEBUG: from client : %s \n", buffer);
            //     if ((*size) == 0)
            //     {
            //         // do noting
            //         printf("Stack is empty.\n");
            //     }
            //     else
            //     {

            //         // char *msg = top(head_stack);
            //         // printf("in top : %s \n", head_stack->txt);
            //         // return head_stack->txt;
            //         printf("DEBUG: TOP value %s\n", head_stack->txt);
            //         send(newSocket, head_stack->txt, 1024, 0);
            //         bzero(buffer, 1024);
            //         // munmap(msg, strlen(msg));
            //     }
            //     bzero(buffer, 1024);
            //     lock.l_type = F_UNLCK;
            //     fcntl(filedescriptor_example, F_SETLKW, &lock);
            //     printf("unlock \n");
            // }
            // else if (strncmp(buffer, "size", 4) == 0)
            // {
            //     // printf("DEBUG: IN SIZE: %s \n", buffer);
            //     // printf("DEBUG: size call \n");
            //     bzero(buffer, sizeof(buffer));
            //     strcat(buffer, "DEBUG:");
            //     sprintf(size_message, "%d", *size);
            //     strncat(buffer, size_message, 1024);
            //     printf("DEBUG: size call %d\n", *size);
            //     send(newSocket, buffer, sizeof(buffer), 0);
            // }

            // else if (strncmp(buffer, "POP", 3) == 0)
            // {

            //     // printf("DEBUG: from client : %s \n", buffer);
            //     if ((*size) == 0)
            //     {
            //         // do noting
            //     }
            //     else
            //     {
            //         // pop(head_stack, *size);
            //         // get temp head node
            //         node_stack_t *temp = head_stack->next;
            //         // set head_stack to head_stack->next
            //         head_stack->next = head_stack->next->next;
            //         // free temp head node txt after allocated with mmap
            //         strcpy(temp->txt, "");
            //         *size = (*size) - 1;
            //     }

            //     // printf("DEBUG: pop good!\n");
            // }

            // if (strncmp(buffer, "hello from ruby \n", 17) == 0) /* hello from ruby \n */
            // {

            //     send(newSocket, buffer_ruby_test, 1024, 0);
            //     bzero(buffer_ruby_test, sizeof(buffer_ruby_test));
            //     sleep(1);
            //     // unlock with fcntl
            //     lock.l_type = F_UNLCK;
            //     fcntl(filedescriptor_example, F_SETLKW, &lock);
            //     close(newSocket);
            //     printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            //     return 0;
            // }
        }
    }
    wait(NULL);
    close(newSocket);

    return 0;
}