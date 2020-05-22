#ifndef SEM_H
#define SEM_H
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#define S 10            //Queue Size
enum semType {SLOTS1, SLOTS2, MSGS1, MSGS2, OSTREAM, MUTEX1, MUTEX2, MSGID, MSGMUTEX};
                        /* Semaphore types for different tasks:
                        *  SLOTS1, SLOTS2 - number of slots in queue available for message input
                        *  MSGS1, MSGS2   - number of msgs in queue for output
                        *  OSTREAM        - mutex for std::cout
                        *  MUTEX1, MUTEX2 - mutex for modifying queues
                        *  MSGID          - keeps track of msg-id produced (32k limit)
                        *  MSGMUTEX       - mutex for creating new message, sync producers
                        */

sem_t* getSems();
void createSems();
void destroySems();

#endif