#ifndef MYSTACK
#define MYSTACK

#include <unistd.h>

struct node_stack
{
    struct node_stack *next;
    char *txt;
};
typedef struct node_stack node_stack_t;


void push(char *str);
void pop();
char *top();

#endif // MYSTACK