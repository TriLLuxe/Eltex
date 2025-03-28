#include "header.h"
#include <stdio.h>
#include <stdlib.h>

PriorityQueue* createPriorityQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (pq == NULL) {perror("Не удалось выделить память"); exit(1);}
    pq->head = NULL;
    return pq;
}
void enqueue (PriorityQueue* pq, int priority, void* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {perror("Не удалось выделить память"); exit(1);}
    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;
    if (pq->head == NULL || priority < pq->head->priority) {
        newNode->next = pq->head;
        pq->head = newNode;
    } else {
        Node* current = pq->head;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void* dequeue (PriorityQueue* pq) {
    if (pq->head == NULL) return NULL;
    Node* temp = pq->head;
    void* data = temp->data;
    pq->head = temp->next;
    free(temp);
    return data;
}
void* dequeueWithPriority(PriorityQueue* pq, int priority) {
    Node* current = pq->head;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->priority == priority) {
            void* data = current->data;
            if (prev == NULL) {
                pq->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return data;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

void* dequeueWithMinPriority(PriorityQueue* pq, int minPriority) {
    Node* current = pq->head;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->priority <= minPriority) {
            void* data = current->data;
            if (prev == NULL) {
                pq->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return data;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

void printQueue(PriorityQueue* pq) {
    Node* current = pq->head;
    printf("Очередь: ");
    while (current != NULL) {
        printf("[%d: %s] -> ", current->priority, (char*)current->data);
        current = current->next;
    }
    printf("NULL\n");
}