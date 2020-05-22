#include "types.h"


Producer::Producer(int id)
{
    msg.prod_id = id; 
    sc = getSems();
}

void Producer::createMsg()
{
    sem_wait(&sc[MSGMUTEX]);                    //block other processes from generating msgs              

    sem_getvalue(&sc[MSGID], &this->msg.msg_id);//get current msg_id
                                                //important: semaphore max value is 32000
    sem_post(&sc[MSGID]);                       //increase msg_id
    sem_post(&sc[MSGMUTEX]);                    //unlock generating msgs

    this->msg.queue_id = rg.genQueueId();       //generate queue id
}

void Producer::work()
{
    Q* q;                                       //queue pointer
    time_t time;                                //time variable for sleeping

    while (true) {
        createMsg();

        if (msg.queue_id == 1)
        {
            sem_wait(&sc[SLOTS1]);              //reserve a slot in queue
            sem_wait(&sc[MUTEX1]);              //block memory from other processes

            q = getQ(1);                        //get queue address
            push(q, msg);                       //push msg to queue


            sem_post(&sc[MSGS1]);               //increase number of elements in queue
            sem_post(&sc[MUTEX1]);              //unlock queue

        }
        else if (msg.queue_id == 2)
        {
            sem_wait(&sc[SLOTS2]);
            sem_wait(&sc[MUTEX2]);

            q = getQ(2);
            push(q, msg);

            
            sem_post(&sc[MSGS2]);
            sem_post(&sc[MUTEX2]);
        }

        sem_wait(&sc[OSTREAM]);                                         //block ostream from other processes

        std::cout << "\nProducer " << msg.prod_id << " created message " << msg.msg_id << " for Q" << msg.queue_id;     //trivial info
        std::this_thread::sleep_for(std::chrono::milliseconds(20));     //a bit more than 1/60s delay to let ostream catch up

        sem_post(&sc[OSTREAM]);                                         //unlock ostream for other processes


        time = rg.getSleepTime(P_WAIT_TIME);
        //std::cout<<"\nP"<<msg.prod_id<<" sleep4 "<<time;
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }
}


Consumer::Consumer(int id)
{
    consumer_id = id;
    sc = getSems();
}

void Consumer::work()
{
    Q *q1 = getQ(1);                            // Queue pointers
    Q *q2 = getQ(2);
    time_t time;                                // Time variable for sleeping
    while (true) {

        sem_wait(&sc[MSGS1]);                   // Wait for available messages in Q1
        sem_wait(&sc[MUTEX1]);                  // Lock Q1 from other processes
        m1 = pop(q1);                           // Pop message from that Q
        sem_post(&sc[SLOTS1]);                  // Increase empty slots in Q1
        sem_post(&sc[MUTEX1]);                  // Unlock Q1 for other processes

        sem_wait(&sc[OSTREAM]);                 // Lock ostream
        std::cout << "\nConsumer "<< consumer_id <<" took message " << m1.msg_id << " from Q1";
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        sem_post(&sc[OSTREAM]);                 // Unlock ostream


        sem_wait(&sc[MSGS2]);
        sem_wait(&sc[MUTEX2]);
        m2 = pop(q2);
        sem_post(&sc[SLOTS2]);
        sem_post(&sc[MUTEX2]);


        sem_wait(&sc[OSTREAM]);
        std::cout << "\nConsumer "<< consumer_id <<" took message " << m2.msg_id << " from Q2";
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        sem_post(&sc[OSTREAM]);

        time = rg.getSleepTime(C_WAIT_TIME);
        //std::cout<<"\nC"<<consumer_id<<" sleep4 "<<time;
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }
}

Reader::Reader(int id)
{ 
    reader_id = id; 
    sc = getSems();
    queue_id = rg.genQueueId(); 
}

void Reader::work()
{

    Q* q = getQ(queue_id);                                  // Queue pointer from which reader will be reading
    int msgs_in_q;                                          // variable to keep track of msgs in that queue
    
    time_t time;
    while (true) {
        
        if(queue_id == 1)
        {
            sem_wait(&sc[MUTEX1]);                          // Lock Queue modifying
            sem_getvalue(&sc[MSGS1], &msgs_in_q);           // Get number of messages in that queue
        }
        else if (queue_id == 2) 
        {
            sem_wait(&sc[MUTEX2]);
            sem_getvalue(&sc[MSGS2], &msgs_in_q);
        }
        else 
        {
            std::cout << "Reader's wrong q\n";
            exit(1);
        }
        if (msgs_in_q > 0) 
        {
            msg.msg_id = q->buf[q->tail-1].msg_id;          // [q->tail - 1] as tail points to next empty space for next message, and
            msg.prod_id = q->buf[q->tail-1].prod_id;        // we want to read the last msg instead of uninitialised memory
            msg.queue_id = q->buf[q->tail-1].queue_id;
        }
        if(queue_id == 1)
            sem_post(&sc[MUTEX1]);                          // Unlock queue
        else
            sem_post(&sc[MUTEX2]);

        sem_wait(&sc[OSTREAM]);                             // Lock Ostream
        std::cout << "\nReader " << reader_id << " read msg "<< msg.msg_id << " from Q"<< queue_id;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        sem_post(&sc[OSTREAM]);                             // Unlock Ostream


        time = rg.getSleepTime(R_WAIT_TIME);
        //std::cout<<"\nR"<<reader_id<<" sleep4 "<<time;
        std::this_thread::sleep_for(std::chrono::seconds(time));

    }
}


RandomGen::RandomGen() : m_mt((std::random_device())()) {}      //inits mersenne twister generator

time_t RandomGen::getSleepTime(int max_wait_time)
{
    std::uniform_int_distribution<int> dist(0, max_wait_time);
    return dist(m_mt);                                          //cast int -> time_t (seconds)
}

int RandomGen::genQueueId()
{
    std::uniform_int_distribution<int> dist(1, 2);
    return dist(m_mt);                                          //returns either queue_id=1 or 2
}