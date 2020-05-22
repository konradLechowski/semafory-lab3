#ifndef QUEUE_H
#define QUEUE_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include "sem.h"
#define NOT_SET -1


struct message
{
    int queue_id;
    int prod_id;
    int msg_id;
};

struct Q
{
    message *buf;
    int head;
    int tail;
    int q_id;
    int max_elements;
};

Q* getQ(int q_num);
void createQ();
void destroyQ();
void printQ();
void push (Q *q, message m);
message pop(Q *queue);

#endif