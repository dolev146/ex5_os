/*
** server.c -- a stream socket server demo
*/
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

#define PORT "5008" // the port users will be connecting to

#define BACKLOG 100 // how many pending connections queue will hold
char size_message[1024];
char client_message[1024];
#define BUFSIZE 1024
#define MAX_LIMIT 1024

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int *size = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *size = 0;

    struct flock *lock = (struct flock *)mmap(NULL, sizeof(struct flock), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // memset(&lock, 0, sizeof(lock)); // We have seen this earlier.
    int *sockfd = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *new_fd = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((*sockfd = socket(p->ai_family, p->ai_socktype,
                              p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(*sockfd);
            perror("farled to server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(*sockfd, BACKLOG) == -1)
    {
        perror("listen failed ");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1)
    { // main accept() loop
        sin_size = sizeof their_addr;
        *new_fd = accept(*sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (*new_fd == -1)
        {
            perror("accept error\n");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork())
        { // this is the child process
            // close(*sockfd); // child doesn't need the listener
            while (true)
            {
                

                recv(*new_fd, client_message, sizeof(client_message), 0);
                // printf("client message: %s and new_fd %d\n", client_message, *new_fd);
                // lock with fcntl to prevent proccesses pushing at the same time
                lock->l_type = F_WRLCK;
                fcntl(*new_fd, F_SETLKW, &*lock);
                if (strncmp(client_message, "PUSH", 4) == 0)
                {
                    // printf("PUSH %c\n", client_message[5]);
                    // printf("DEBUG:from client : %s\n", client_message);
                    memcpy(client_message, client_message + 5, MAX_LIMIT - 5);
                    push(client_message);
                    *size = *size + 1;
                    printf("size in push: %d\n", *size);

                    // printf("server: pushed and new_fd is %d\n", *new_fd);
                    // sleep(5);
                    // printf("DEBUG: push good! socket %d\n", client_socket);
                }
                else if (strncmp(client_message, "POP", 3) == 0)
                {

                    // printf("DEBUG: from client : %s \n", client_message);
                    pop();
                    *size = *size - 1;
                    // printf("DEBUG: pop good!\n");
                }
                else if (strncmp(client_message, "TOP", 3) == 0)
                {
                    // printf("DEBUG: from client : %s \n", client_message);
                    char *msg = top();
                    send(*new_fd, msg, sizeof(msg), 0);
                    free(msg);
                }
                else if (strncmp(client_message, "size", 4) == 0)
                {
                    // printf("DEBUG: IN SIZE: %s \n", client_message);
                    printf("DEBUG: size call \n");
                    bzero(client_message, sizeof(client_message));
                    printf("got to line 197\n");
                    strcat(client_message, "DEBUG:");
                    printf("got to line 198\n");
                    sprintf(size_message, "%d", *size);
                    printf("got to line 200\n");
                    strncat(client_message, size_message, sizeof(size_message));
                    printf("got to line 203\n");
                    printf("DEBUG: size call good! %d\n", *size);
                    sleep(5);
                    send(*new_fd, client_message, sizeof(client_message), 0);
                }
                else if (strncmp(client_message, "exit", 4) == 0)
                {
                    // unlock with fcntl
                    lock->l_type = F_UNLCK;
                    fcntl(*new_fd, F_SETLKW, &*lock);
                    close(*new_fd);
                    break;
                }

                if (strncmp(client_message, "hello from ruby \n", 17) == 0) /* hello from ruby \n */
                {
                    char buffer_ruby_test[BUFSIZE] = "hi from server ";
                    printf("%s", client_message);
                    send(*new_fd, buffer_ruby_test, 1024, 0);
                    bzero(buffer_ruby_test, sizeof(buffer_ruby_test));
                    close(*new_fd);
                    // unlock with fcntl
                    lock->l_type = F_UNLCK;
                    fcntl(*new_fd, F_SETLKW, &lock);
                    break;
                }
                // unlock with fcntl
                lock->l_type = F_UNLCK;
                // printf("new fd is %d\n", *new_fd);
                fcntl(*new_fd, F_SETLKW, &*lock);
                // printf("DEBUG: unlock good!\n");
            }
            // sleep(10);
        }
    }
    close(*new_fd); // parent doesn't need this
    return 0;
}