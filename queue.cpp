#include "queue.h"

int q_mem_id_1 = 0;
int q_mem_id_2 = 0;

Q* getQ(int q_num)
{   
    if (q_num == 1)
    {
        if(q_mem_id_1 == 0)                                                                 // Memory not set yet (first run)
            q_mem_id_1 = shmget(IPC_PRIVATE, sizeof(Q)+(S)*sizeof(message), SHM_W|SHM_R);   // Create shared memory and set id

        if (q_mem_id_1 < 0)                                                                 // Throw error if memory assignment failed
        {
            std::cout << "q_mem_id1\n";
            exit(1);
        }

        void *m_ptr_1 = shmat(q_mem_id_1, NULL, 0);                                         // New memory pointer to shared memory

        if (m_ptr_1 <= 0)                                                                   // Throw error if m_ptr points to nothing
        {
            std::cout << "m_ptr_1\n";
            exit(1);
        }

        return (Q*)m_ptr_1;
    }
    else if (q_num == 2)
    {
        if(q_mem_id_2 == 0)                                                                 
            q_mem_id_2 = shmget(IPC_PRIVATE, sizeof(Q)+(S)*sizeof(message), SHM_W|SHM_R);   

        if (q_mem_id_2 < 0)                                                                 
            exit(2);

        void *m_ptr_2 = shmat(q_mem_id_2, NULL, 0);

        if (m_ptr_2 <= 0)
            exit(3);

        return (Q*)m_ptr_2;
    }
    else                                                                                    // Invalid queue id provided
    {
        std::cout <<"getQ invalid arg\n";
        exit(1);
    }
}

void createQ()
{
    Q* queue_1 = getQ(1);                                       // Get pointer to queue_1
    queue_1->buf = (message*)((void*)queue_1+sizeof(Q));        // Make buf point to message in message space, after other variables from struct Q
    memset(queue_1->buf, NOT_SET, S*sizeof(message));           // Set all message shared memory to -1 == NOT_SET 
    queue_1->head = 0;                                          // Set variables
    queue_1->tail = 0;
    queue_1->max_elements = S;
    queue_1->q_id = 1;


    Q* queue_2 = getQ(2);
    queue_2->buf = (message*)((void*)queue_2+sizeof(Q));
    memset(queue_2->buf, NOT_SET, S*sizeof(message));
    queue_2->head = 0;
    queue_2->tail = 0;
    queue_2->max_elements = S;
    queue_2->q_id = 2;
 
}

void destroyQ()
{
    Q* q1 = getQ(1);                                        //get pointer to queue_1
    shmdt(q1);                                              //detach queue_1 from address space of the calling process
    shmctl(q_mem_id_1, IPC_RMID, NULL);                     //set memory to be destroyed

    Q* q2 = getQ(2);                                        
    shmdt(q2);
    shmctl(q_mem_id_2, IPC_RMID, NULL);
}

void push(Q *queue, message m)
{
    queue->buf[queue->tail].msg_id = m.msg_id;              //insert message at tail's address
    queue->buf[queue->tail].prod_id = m.prod_id;
    queue->buf[queue->tail].queue_id = m.queue_id;
    queue->tail++;                                          //increase tail's pointer
    queue->tail %= queue->max_elements;                     //loop it back if exceeds memory boundaries
}

message pop(Q* queue)
{
    message temp;
    temp.msg_id = queue->buf[queue->head].msg_id;           // get message full info
    temp.prod_id = queue->buf[queue->head].prod_id;
    temp.queue_id = queue->buf[queue->head].queue_id;

    queue->buf[queue->head].msg_id = NOT_SET;               // reset memory for that element
    queue->buf[queue->head].prod_id = NOT_SET;
    queue->buf[queue->head].queue_id = NOT_SET;

    queue->head++;                                          //move head to the next mem segment
    queue->head %= queue->max_elements;                     //make sure it does not leave memory boundaries

    return temp;                                            
}