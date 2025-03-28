

#ifndef HEADER_H
#define HEADER_H
typedef struct Node {
    int priority;
    void *data;
    struct Node *next;
} Node;
typedef struct PriorityQueue {
    Node* head;
} PriorityQueue;

PriorityQueue* createPriorityQueue();
void enqueue(PriorityQueue* pq, int priority, void* data);
void* dequeue(PriorityQueue* pq);
void* dequeueWithPriority(PriorityQueue* pq, int priority);
void* dequeueWithMinPriority(PriorityQueue* pq, int minPriority);
void printQueue(PriorityQueue* pq);
#endif //HEADER_H
