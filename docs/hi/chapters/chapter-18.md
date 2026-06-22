# Chapter 18 — Data Structures in C

Data structures data को organize करने के तरीके हैं। C में इन्हें structs और pointers की मदद से manually implement करते हैं।

## Array

Array fixed-size contiguous memory देता है। Fast indexing मिलती है, लेकिन size fixed होता है।

## Linked List

Linked list nodes से बनती है। हर node में data और next node का pointer होता है।

```c
struct Node {
    int data;
    struct Node *next;
};
```

Linked list dynamic size के लिए useful है।

## Stack

Stack LIFO principle follow करता है: Last In, First Out.

Common operations:

- `push`
- `pop`
- `peek`

## Queue

Queue FIFO principle follow करता है: First In, First Out.

Common operations:

- `enqueue`
- `dequeue`

## Tree

Tree hierarchical data store करता है। Binary tree में हर node के maximum two children होते हैं।

```c
struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
};
```

## Practice

1. Array-based stack implement करें।
2. Linked list में insert और print operations बनाएं।
3. Queue का simple implementation लिखें।
