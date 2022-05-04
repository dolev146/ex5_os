#ifndef MYSTACK
#define MYSTACK

#include <unistd.h>

struct node_stack
{
    struct node_stack *next;
    char txt[1024];
};
typedef struct node_stack node_stack_t;

char *push(char *str, node_stack_t *head_stack,node_stack_t *new_node);
void pop(node_stack_t *head_stack, int size);
char *top(node_stack_t *head_stack);
void print_stack(node_stack_t *head_stack);
void init_stack(node_stack_t *head_param);

#endif // MYSTACK