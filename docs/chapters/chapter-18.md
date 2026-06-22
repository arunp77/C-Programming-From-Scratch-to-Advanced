# Chapter 18 — Data Structures in C

## Introduction

Many languages ship with built-in container types — a vector, a list, a dictionary or map — ready to use out of the box. C does not. The standard library gives you arrays, and that is essentially it. If you need a structure that grows and shrinks dynamically, that maintains insertion order with efficient insertion at both ends, or that keeps data sorted for fast lookup, you build it yourself out of structs and pointers.

This is not a gap to be annoyed by; it is one of the most valuable things C teaches. Every higher-level language's "list" or "map" is, underneath, an implementation of one of the structures in this chapter, written in C or a C-like language. Understanding linked lists, stacks, queues, and trees by hand is what makes those higher-level containers stop feeling like magic.

This chapter relies directly on structs and pointers (Chapters 12, 10–11), dynamic memory management (Chapter 13), and recursion (Chapter 16). If any of those feel shaky, this is a good moment to revisit them — every structure here is built from exactly those pieces.

This is not an algorithms course, so Big-O notes are kept brief — one line or a small table per structure — but they are included because knowing the *cost* of an operation is part of knowing the structure.

## Singly Linked Lists

A singly linked list is a chain of nodes, where each node holds a piece of data and a pointer to the next node in the chain. Unlike an array, a linked list's nodes are not contiguous in memory and the list can grow one node at a time without ever needing to "resize" anything.

```c
struct Node {
    int data;
    struct Node *next;
};
```

The list itself is typically represented by nothing more than a pointer to its first node, called the **head**. An empty list is simply a head pointer equal to `NULL`.

### Core operations, fully implemented

```c
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node *next;
};

/* Allocates and initializes a single new node. */
struct Node *create_node(int data) {
    struct Node *node = malloc(sizeof(struct Node));
    if (node == NULL) {
        fprintf(stderr, "create_node: allocation failed\n");
        exit(1);
    }
    node->data = data;
    node->next = NULL;
    return node;
}

/* Inserts a new node at the front of the list. O(1). */
void insert_at_head(struct Node **head, int data) {
    struct Node *node = create_node(data);
    node->next = *head;
    *head = node;
}

/* Inserts a new node at the end of the list. O(n), must walk to the end. */
void insert_at_tail(struct Node **head, int data) {
    struct Node *node = create_node(data);
    if (*head == NULL) {
        *head = node;
        return;
    }
    struct Node *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = node;
}

/* Prints the list from head to tail. O(n). */
void print_list(const struct Node *head) {
    const struct Node *current = head;
    printf("[ ");
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("]\n");
}

/* Returns 1 if value is found, 0 otherwise. O(n). */
int search(const struct Node *head, int value) {
    const struct Node *current = head;
    while (current != NULL) {
        if (current->data == value) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/* Deletes the first node matching value, if any. O(n). */
void delete_value(struct Node **head, int value) {
    struct Node *current = *head;
    struct Node *previous = NULL;

    while (current != NULL) {
        if (current->data == value) {
            if (previous == NULL) {
                *head = current->next; /* deleting the head node */
            } else {
                previous->next = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
    /* value not found: nothing to delete */
}

/* Frees every node in the list. Essential to avoid the leaks
 * discussed in Chapter 13 — a linked list does not free itself. */
void free_list(struct Node **head) {
    struct Node *current = *head;
    while (current != NULL) {
        struct Node *next = current->next; /* save next before freeing current */
        free(current);
        current = next;
    }
    *head = NULL;
}

int main(void) {
    struct Node *head = NULL;

    insert_at_tail(&head, 10);
    insert_at_tail(&head, 20);
    insert_at_tail(&head, 30);
    insert_at_head(&head, 5);

    printf("List after inserts: ");
    print_list(head);

    printf("Search 20: %s\n", search(head, 20) ? "found" : "not found");
    printf("Search 99: %s\n", search(head, 99) ? "found" : "not found");

    delete_value(&head, 20);
    printf("List after deleting 20: ");
    print_list(head);

    free_list(&head);
    printf("List after free_list, head is %s\n", head == NULL ? "NULL" : "not NULL");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra linked_list.c -o linked_list
./linked_list
```

### Expected output

```text
List after inserts: [ 5 10 20 30 ]
Search 20: found
Search 99: not found
List after deleting 20: [ 5 10 30 ]
List after free_list, head is NULL
```

Note that every list-modifying function takes `struct Node **head` — a pointer to the head pointer. This is exactly the pointer-to-pointer pattern from Chapter 17: `insert_at_head` and `delete_value` need to be able to change *which node the caller's head pointer points to* (for example, making it point to a new first node, or to `NULL` when the list becomes empty), and that requires the address of the head pointer itself.

| Operation | Cost |
|---|---|
| Insert at head | O(1) |
| Insert at tail | O(n) (O(1) if a tail pointer is also maintained) |
| Search | O(n) |
| Delete by value | O(n) |

## Stacks (LIFO)

A stack is a Last-In-First-Out structure: the most recently added item is the first one removed. The four canonical operations are `push` (add), `pop` (remove and return the top), `peek` (look at the top without removing it), and checks for empty/full.

### Array-based stack

```c
#include <stdio.h>

#define STACK_CAPACITY 100

typedef struct {
    int data[STACK_CAPACITY];
    int top; /* index of the topmost element; -1 means empty */
} ArrayStack;

void stack_init(ArrayStack *stack) {
    stack->top = -1;
}

int stack_is_empty(const ArrayStack *stack) {
    return stack->top == -1;
}

int stack_is_full(const ArrayStack *stack) {
    return stack->top == STACK_CAPACITY - 1;
}

int stack_push(ArrayStack *stack, int value) {
    if (stack_is_full(stack)) {
        return 0; /* failure: no room */
    }
    stack->top++;
    stack->data[stack->top] = value;
    return 1;
}

int stack_pop(ArrayStack *stack, int *out_value) {
    if (stack_is_empty(stack)) {
        return 0; /* failure: nothing to pop */
    }
    *out_value = stack->data[stack->top];
    stack->top--;
    return 1;
}

int stack_peek(const ArrayStack *stack, int *out_value) {
    if (stack_is_empty(stack)) {
        return 0;
    }
    *out_value = stack->data[stack->top];
    return 1;
}

int main(void) {
    ArrayStack stack;
    stack_init(&stack);

    stack_push(&stack, 1);
    stack_push(&stack, 2);
    stack_push(&stack, 3);

    int top_value;
    stack_peek(&stack, &top_value);
    printf("Peek: %d\n", top_value);

    int value;
    while (stack_pop(&stack, &value)) {
        printf("Popped: %d\n", value);
    }

    if (!stack_pop(&stack, &value)) {
        printf("Stack is empty, pop failed as expected\n");
    }

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra array_stack.c -o array_stack
./array_stack
```

### Expected output

```text
Peek: 3
Popped: 3
Popped: 2
Popped: 1
Stack is empty, pop failed as expected
```

### Linked-list-based stack

A stack built on a linked list pushes by inserting at the head and pops by removing the head — both O(1), and with no fixed capacity.

```c
#include <stdio.h>
#include <stdlib.h>

struct StackNode {
    int data;
    struct StackNode *next;
};

typedef struct {
    struct StackNode *top;
} LinkedStack;

void stack_init(LinkedStack *stack) {
    stack->top = NULL;
}

int stack_is_empty(const LinkedStack *stack) {
    return stack->top == NULL;
}

void stack_push(LinkedStack *stack, int value) {
    struct StackNode *node = malloc(sizeof(struct StackNode));
    if (node == NULL) {
        fprintf(stderr, "stack_push: allocation failed\n");
        exit(1);
    }
    node->data = value;
    node->next = stack->top;
    stack->top = node;
}

int stack_pop(LinkedStack *stack, int *out_value) {
    if (stack_is_empty(stack)) {
        return 0;
    }
    struct StackNode *old_top = stack->top;
    *out_value = old_top->data;
    stack->top = old_top->next;
    free(old_top);
    return 1;
}

void stack_free(LinkedStack *stack) {
    int discard;
    while (stack_pop(stack, &discard)) {
        /* keep popping until empty, freeing every node */
    }
}

int main(void) {
    LinkedStack stack;
    stack_init(&stack);

    stack_push(&stack, 100);
    stack_push(&stack, 200);
    stack_push(&stack, 300);

    int value;
    while (stack_pop(&stack, &value)) {
        printf("Popped: %d\n", value);
    }

    stack_free(&stack); /* safe even though already empty */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra linked_stack.c -o linked_stack
./linked_stack
```

### Expected output

```text
Popped: 300
Popped: 200
Popped: 100
```

**Array vs. linked-list stack:** the array version has a fixed capacity decided up front and no per-element overhead, but wastes space if undersized or rejects pushes once full. The linked-list version has no fixed limit (besides available memory) but pays for a pointer in every node and an allocation/free on every push/pop.

| Operation | Cost (both implementations) |
|---|---|
| Push | O(1) |
| Pop | O(1) |
| Peek | O(1) |

## Queues (FIFO)

A queue is First-In-First-Out: the earliest-added item is the first removed. The two implementations below mirror the stack section: a fixed-capacity array version and an unbounded linked-list version.

### Circular array queue

A naive array queue that always dequeues from index 0 would need to shift every remaining element down on every dequeue — O(n) instead of O(1). A **circular buffer** avoids this by letting the front and back indices wrap around the array using the modulo operator, so both enqueue and dequeue stay O(1).

```c
#include <stdio.h>

#define QUEUE_CAPACITY 5

typedef struct {
    int data[QUEUE_CAPACITY];
    int front;
    int count; /* number of elements currently stored */
} CircularQueue;

void queue_init(CircularQueue *queue) {
    queue->front = 0;
    queue->count = 0;
}

int queue_is_empty(const CircularQueue *queue) {
    return queue->count == 0;
}

int queue_is_full(const CircularQueue *queue) {
    return queue->count == QUEUE_CAPACITY;
}

int queue_enqueue(CircularQueue *queue, int value) {
    if (queue_is_full(queue)) {
        return 0;
    }
    int back = (queue->front + queue->count) % QUEUE_CAPACITY;
    queue->data[back] = value;
    queue->count++;
    return 1;
}

int queue_dequeue(CircularQueue *queue, int *out_value) {
    if (queue_is_empty(queue)) {
        return 0;
    }
    *out_value = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_CAPACITY;
    queue->count--;
    return 1;
}

int main(void) {
    CircularQueue queue;
    queue_init(&queue);

    for (int i = 1; i <= 5; i++) {
        printf("enqueue(%d) -> %s\n", i, queue_enqueue(&queue, i) ? "ok" : "full!");
    }
    printf("enqueue(6) -> %s (queue is full)\n",
           queue_enqueue(&queue, 6) ? "ok" : "full!");

    int value;
    queue_dequeue(&queue, &value);
    printf("dequeued: %d\n", value);
    queue_dequeue(&queue, &value);
    printf("dequeued: %d\n", value);

    /* Now there is room again; this enqueue wraps around to the front
     * of the underlying array — this is the "circular" part. */
    queue_enqueue(&queue, 6);
    printf("enqueued 6 (wrapped around)\n");

    printf("Remaining: ");
    while (queue_dequeue(&queue, &value)) {
        printf("%d ", value);
    }
    printf("\n");

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra circular_queue.c -o circular_queue
./circular_queue
```

### Expected output

```text
enqueue(1) -> ok
enqueue(2) -> ok
enqueue(3) -> ok
enqueue(4) -> ok
enqueue(5) -> ok
enqueue(6) -> full! (queue is full)
dequeued: 1
dequeued: 2
enqueued 6 (wrapped around)
Remaining: 3 4 5 6 
```

!!! warning "The full-vs-empty ambiguity"
    A circular buffer that tracks only a `front` and a `back` index (no separate count) cannot distinguish "completely empty" from "completely full" — in both cases `front == back`. There are two standard fixes: track the element `count` separately (the approach used above), or deliberately leave one slot always empty so `front == back` can only mean "empty." Tracking `count` is simpler to reason about and is the more common approach in practice.

### Linked-list queue (head and tail pointers)

A linked-list queue enqueues at the tail and dequeues at the head, so it needs to track both ends. This gives O(1) enqueue and dequeue with no fixed capacity.

```c
#include <stdio.h>
#include <stdlib.h>

struct QueueNode {
    int data;
    struct QueueNode *next;
};

typedef struct {
    struct QueueNode *head;
    struct QueueNode *tail;
} LinkedQueue;

void queue_init(LinkedQueue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

int queue_is_empty(const LinkedQueue *queue) {
    return queue->head == NULL;
}

void queue_enqueue(LinkedQueue *queue, int value) {
    struct QueueNode *node = malloc(sizeof(struct QueueNode));
    if (node == NULL) {
        fprintf(stderr, "queue_enqueue: allocation failed\n");
        exit(1);
    }
    node->data = value;
    node->next = NULL;

    if (queue->tail == NULL) {
        /* queue was empty: new node is both head and tail */
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
}

int queue_dequeue(LinkedQueue *queue, int *out_value) {
    if (queue_is_empty(queue)) {
        return 0;
    }
    struct QueueNode *old_head = queue->head;
    *out_value = old_head->data;
    queue->head = old_head->next;
    if (queue->head == NULL) {
        queue->tail = NULL; /* queue became empty */
    }
    free(old_head);
    return 1;
}

void queue_free(LinkedQueue *queue) {
    int discard;
    while (queue_dequeue(queue, &discard)) {
        /* keep dequeuing until empty, freeing every node */
    }
}

int main(void) {
    LinkedQueue queue;
    queue_init(&queue);

    queue_enqueue(&queue, 10);
    queue_enqueue(&queue, 20);
    queue_enqueue(&queue, 30);

    int value;
    while (queue_dequeue(&queue, &value)) {
        printf("dequeued: %d\n", value);
    }

    queue_free(&queue); /* safe no-op here; demonstrates cleanup discipline */

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra linked_queue.c -o linked_queue
./linked_queue
```

### Expected output

```text
dequeued: 10
dequeued: 20
dequeued: 30
```

| Operation | Circular array queue | Linked-list queue |
|---|---|---|
| Enqueue | O(1) | O(1) |
| Dequeue | O(1) | O(1) |
| Capacity | Fixed | Unbounded |
| Overhead | None per element | One pointer per node |

## Binary Trees and Binary Search Trees

A binary tree node holds data plus pointers to (at most) two children, conventionally called `left` and `right`.

```c
struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
};
```

A **binary search tree (BST)** adds an ordering rule: for every node, all values in its left subtree are smaller, and all values in its right subtree are larger. This rule is what makes searching a BST fast — at each node you can discard an entire subtree based on a single comparison.

### BST insertion and in-order traversal

Both insertion and traversal are naturally recursive, directly applying the recursion concepts from Chapter 16: each function handles one node and then delegates to itself for the subtrees, with `NULL` (an empty subtree) as the base case.

```c
#include <stdio.h>
#include <stdlib.h>

struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
};

struct TreeNode *create_tree_node(int data) {
    struct TreeNode *node = malloc(sizeof(struct TreeNode));
    if (node == NULL) {
        fprintf(stderr, "create_tree_node: allocation failed\n");
        exit(1);
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* Inserts value into the BST rooted at root, returning the (possibly
 * new) root. Recursive base case: an empty subtree (NULL) becomes a
 * new leaf node. */
struct TreeNode *bst_insert(struct TreeNode *root, int value) {
    if (root == NULL) {
        return create_tree_node(value);
    }

    if (value < root->data) {
        root->left = bst_insert(root->left, value);
    } else if (value > root->data) {
        root->right = bst_insert(root->right, value);
    }
    /* equal values are ignored here; a real-world tree might count
     * duplicates instead */

    return root;
}

/* In-order traversal visits left subtree, then this node, then right
 * subtree -- for a BST this always prints values in sorted order. */
void bst_print_inorder(const struct TreeNode *root) {
    if (root == NULL) {
        return; /* base case: nothing to print */
    }
    bst_print_inorder(root->left);
    printf("%d ", root->data);
    bst_print_inorder(root->right);
}

/* Recursively frees every node. Post-order shape: free children
 * before freeing the node itself. */
void bst_free(struct TreeNode *root) {
    if (root == NULL) {
        return;
    }
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

int main(void) {
    struct TreeNode *root = NULL;
    int values[] = { 50, 30, 70, 20, 40, 60, 80 };
    size_t count = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < count; i++) {
        root = bst_insert(root, values[i]);
    }

    printf("In-order traversal (sorted): ");
    bst_print_inorder(root);
    printf("\n");

    bst_free(root);

    return 0;
}
```

### Compile and run

```bash
gcc -std=c11 -Wall -Wextra bst.c -o bst
./bst
```

### Expected output

```text
In-order traversal (sorted): 20 30 40 50 60 70 80 
```

In-order traversal of a BST always produces values in sorted order, regardless of insertion order — a direct consequence of the BST ordering rule. **Pre-order** traversal (visit the node, then left, then right) and **post-order** traversal (visit left, then right, then the node) are the other two standard depth-first traversals; both follow the same recursive shape as `bst_print_inorder` with only the position of the "visit this node" step moved. Implementing both is left as an exercise.

| Operation | Average case (balanced) | Worst case (degenerate, list-like) |
|---|---|---|
| Search | O(log n) | O(n) |
| Insert | O(log n) | O(n) |
| Delete | O(log n) | O(n) |

The worst case arises when values are inserted in sorted order, producing a tree that is really just a linked list in disguise — balancing strategies that avoid this exist (AVL trees, red-black trees) but are beyond this tutorial's scope.

## Summary

- C provides no built-in container types; structures like linked lists, stacks, queues, and trees must be built from structs and pointers, which is also exactly what higher-level languages' built-in containers do internally.
- A singly linked list chains nodes via a `next` pointer; head-pointer-modifying operations (insert at head, delete) require a pointer-to-pointer (`struct Node **head`) for the same reason covered in Chapter 17.
- A stack (LIFO) can be implemented on a fixed-size array (push/pop/peek/is_empty/is_full via a `top` index) or on a linked list (push/pop at the head); the array version has a capacity limit and no per-node overhead, the linked-list version is unbounded but pays pointer overhead.
- A queue (FIFO) can be implemented as a circular array buffer (wraparound via `% capacity`, with a separate `count` to resolve the full-vs-empty ambiguity) or as a linked list with head and tail pointers (enqueue at tail, dequeue at head).
- A binary search tree maintains the invariant that left-subtree values are smaller and right-subtree values are larger than each node, enabling average O(log n) search/insert/delete; in-order traversal of a BST always yields sorted output.
- Every dynamically allocated structure in this chapter needs an explicit "free everything" routine — leaking nodes is exactly the Chapter 13 leak problem, just spread across many small allocations instead of one.

## Exercises

1. Extend the singly linked list with an `insert_at_position(struct Node **head, int position, int data)` function that inserts at an arbitrary zero-based index.
2. Implement a doubly linked list (each node has both `next` and `prev` pointers) supporting insertion and deletion from either end in O(1).
3. Using the array-based stack, write a balanced-parentheses checker: read a string of `(`, `)`, `{`, `}`, `[`, `]` characters and report whether the brackets are properly balanced and nested.
4. Implement `bst_search(const struct TreeNode *root, int value)` returning 1 if found, 0 otherwise, and `bst_height(const struct TreeNode *root)` returning the tree's height recursively.
5. *(Harder)* Implement pre-order and post-order traversal functions for the BST from this chapter, and use one of them (which one, and why?) to write a `bst_copy` function that produces a complete, independent deep copy of a tree.
6. *(Harder)* Implement BST node deletion (`bst_delete(struct TreeNode *root, int value)`), handling all three cases: deleting a leaf, deleting a node with one child, and deleting a node with two children (hint: replace the deleted node's value with its in-order successor, then delete that successor node instead).

---
[← Chapter 17 — Advanced Pointers](chapter-17.md) | [Chapter 19 — Multi-File Projects, Header Files, and Build Systems →](chapter-19.md)
