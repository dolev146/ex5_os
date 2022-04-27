#include "mystack.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//include mmap header
#include <sys/mman.h>


// https://stackoverflow.com/questions/56606496/how-to-add-a-string-to-linked-list-in-c

// allocate memory for head_stack with shared memory and set it to NULL
node_stack_t *head_stack = (node_stack_t *)mmap(NULL, sizeof(node_stack_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
// init integer named size that will be used to keep track of the size of the stack with mmap shared memory



void push(char* str){
    // allocate memory for new node with shared memory
    node_stack_t *new_node = (node_stack_t *)mmap(NULL, sizeof(node_stack_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // set new node's txt to str using mmap with shared memroy
    new_node->txt = (char *)mmap(NULL, strlen(str) + 1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // copy str to new node's txt
    strcpy(new_node->txt, str);
    // set new node's next to head_stack
    new_node->next = head_stack;
    // set head_stack to new node
    head_stack = new_node;
}


// node_stack_t *head_stack = NULL;


// void push(char *str)
// {
    
//     node_stack_t *newnode = (node_stack_t *)malloc(sizeof(node_stack_t));
//     // step 1. allocate memory to hold word
//     newnode->txt = (char *)malloc(strlen(str) + 1);
//     // step 2. copy the current word
//     strcpy(newnode->txt, str);
//     newnode->next = NULL;
//     if (head_stack == NULL)
//     {
//         head_stack = newnode;
//     }
//     else
//     {
//         newnode->next = head_stack;
//         head_stack = newnode;
//     }
//     size++;
// }

void pop(){
    // if head_stack is NULL, return
    if (head_stack == NULL)
    {
        return;
    }
    // get temp head node
    node_stack_t *temp = head_stack;
    // set head_stack to head_stack->next
    head_stack = head_stack->next;
    // free temp head node txt after allocated with mmap
    munmap(temp->txt, strlen(temp->txt) + 1);
    // free temp head node after allocated with mmap
    munmap(temp, sizeof(node_stack_t));
}

char *top()
{
    char *msg = (char *)malloc(strlen(head_stack->txt) + 50);
    strcpy(msg, "OUTPUT:");
    strcat(msg, head_stack->txt);
    printf("%s", msg);
    return msg;
}

