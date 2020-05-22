#ifndef TYPES_H
#define TYPES_H
#include <semaphore.h> 
#include <fcntl.h> 
#include <chrono>
#include <random>
#include "queue.h"
#include "sem.h"
#define R_WAIT_TIME 30      //max sleep time for reader
#define P_WAIT_TIME 30      //max sleep time for producer
#define C_WAIT_TIME 30      //max sleep time for consumer


class RandomGen {
    private:
        std::mt19937 m_mt;
    public:
        RandomGen();
        ~RandomGen() {};

        time_t getSleepTime(int max_wait_time);         //generates sleep time in seconds for every person
        int genQueueId();                               //generates and returns either '1' or '2'

};

class Producer
{
    private:
        RandomGen rg;
        sem_t* sc;
        message msg;
    public:
        Producer(int id);
        void work();
        void createMsg();
};

class Consumer
{
    private:
        int consumer_id;
        RandomGen rg;
        sem_t* sc;
        message m1;
        message m2;
    public:
        Consumer(int id);
        void work();
};

class Reader
{
    private:
        int reader_id;
        int queue_id;
        RandomGen rg;
        sem_t* sc;
        message msg;
    public:
        Reader(int id);
        void work();
};

#endif