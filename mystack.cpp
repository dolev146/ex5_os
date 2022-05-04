#include "mystack.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// include mmap header
#include <sys/mman.h>

// https://stackoverflow.com/questions/56606496/how-to-add-a-string-to-linked-list-in-c

// allocate memory for head_stack with shared memory and set it to NULL

// init integer named size that will be used to keep track of the size of the stack with mmap shared memory

char *push(char *str, node_stack_t *head_stack, node_stack_t *new_node)
{
    // allocate memory for new node with shared memory
    // node_stack_t *new_node = (node_stack_t *)mmap(NULL, sizeof(node_stack_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // copy str to new node's txt
    // printf("[+]Pushing %s to stack.\n", str);
    strcpy(new_node->txt, str);
    printf("DEBUG:[+]Pushing %s to stack.\n", new_node->txt);
    // set new node's next to head_stack
    new_node->next = head_stack;
    // set head_stack to new node
    head_stack = new_node;
    return head_stack->txt;
}

void pop(node_stack_t *head_stack, int size)
{
    // if head_stack is NULL, return
    if (size == 0)
    {
        return;
    }
    // get temp head node
    node_stack_t *temp = head_stack->next;
    // set head_stack to head_stack->next
    head_stack->next = head_stack->next->next;
    // free temp head node txt after allocated with mmap
    strcpy(temp->txt, "");
    // free temp head node after allocated with mmap
    munmap(temp, sizeof(node_stack_t));
}

char *top(node_stack_t *head_stack)
{
    // char *msg = (char *)mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // strcpy(msg, "OUTPUT:");
    // strcat(msg, head_stack->txt);
    printf("in top : %s \n", head_stack->txt);
    return head_stack->txt;
}

void print_stack(node_stack_t *head_stack)
{
    node_stack_t *temp = head_stack;
    while (temp->txt[0] != '\0')
    {
        printf("%s\n", temp->txt);
        temp = temp->next;
    }
}