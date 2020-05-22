#include "sem.h"


int sem_m_id = 0;

sem_t* getSems()
{
    if (sem_m_id == 0)                                                  // first run
        sem_m_id = shmget(IPC_PRIVATE, 9*sizeof(sem_t), SHM_W|SHM_R);   // create shared memory space for semaphores

    if (sem_m_id < 0)                                                   // if mem allocation failed
    {
        std::cout << "Sem m id\n";
        exit(1);
    }

    void* m_ptr = shmat(sem_m_id, NULL, 0);                             // pointer to shared memory

    if (m_ptr < 0)                                                      // if getting semaphore shared memory address failed
    {
        std::cout << "sem m_ptr\n";
        exit(1);
    }
    return (sem_t*)m_ptr;                                               // return pointer to semaphores (casted at semaphores)
}

void createSems()
{
    sem_t* semaphores = getSems();                                      // create shm for semaphores and get pointer to that memory
    sem_init(&semaphores[SLOTS1], 1, S);
    sem_init(&semaphores[SLOTS2], 1, S);
    sem_init(&semaphores[MSGS1],  1, 0);
    sem_init(&semaphores[MSGS2],  1, 0);
    sem_init(&semaphores[OSTREAM],1, 1);
    sem_init(&semaphores[MUTEX1], 1, 1);
    sem_init(&semaphores[MUTEX2], 1, 1);
    sem_init(&semaphores[MSGID],  1, 0);                                // Semaphore value limit of 32k
    sem_init(&semaphores[MSGMUTEX], 1, 1);
}


void destroySems()
{
    sem_t* semaphores = getSems();                                      // get pointer to semaphores
    shmdt(semaphores);                                                  // detach semaphores from the address space of the calling process
    shmctl(sem_m_id, IPC_RMID, NULL);                                   // set memory to be gone, reduced to atoms
}