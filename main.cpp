#include <iostream>         //std::cout
#include <semaphore.h>      //sem_t
#include "types.h"
#include "queue.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>         //fork()
#include <fcntl.h>          // O_CREAT etc


#define R 3                 //reader count
#define P 5                 //producer count
#define C 2                 //consumer count

sem_t *total_processes;     //Number of processes left to create
sem_t *SOI_init;            //Sync creating new processes
void close_sem();


int main (int argc, char *argv[])
{
    int i;
    int current_type;
    pid_t pid;
    
    close_sem();    //close previous semaphores
    createQ();
    

    total_processes = sem_open("/soi_total_processes", O_CREAT | O_EXCL, 0644, R+P+C);
    SOI_init = sem_open("/soi_init", O_CREAT | O_EXCL, 0644, 1);

    createSems();
    sem_t *sems = getSems();

    current_type = 0;
    for (i = 0; i < P+C+R; i++)                 //spawn children
    {
        pid = fork();

        if(pid < 0)
        {
            close_sem();
            std::cout <<"Fatal error on fork";
            exit(1);                            //parent exits
        }
        else if (pid == 0)                      //child is here after fork()
        {
            SOI_init = sem_open("/soi_init", O_CREAT);
            total_processes = sem_open("/soi_total_processes", O_CREAT);    //open sems again, as children don't inherit parent's semaphores
            break;
        }
    }

    if (pid != 0)                               //running in the parent process, after spawning children
    {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2000));
            std::cout << "init shutdown";
        }
    }
    
    if(pid == 0) 
    {
        sem_wait(SOI_init);
        sem_getvalue(total_processes, &current_type);
        
        int child_type = current_type;
        if ( i < P)
        {

            Producer *p = new Producer( i );

            sem_wait(total_processes);  //decrease number of people to init
            sem_post(SOI_init);         //unlock for next 
            p->work();
            delete p;
        }
        else if ( i >= P && i < P+C)
        {
            Consumer *c = new Consumer( i - P );
            sem_wait(total_processes);  //decrease number of people to init
            sem_post(SOI_init);         //unlock for next 
            c->work();
            delete c;
        }
        else if (i >= P+C)
        {
            Reader *r = new Reader( P + C + R - i - 1);
            sem_wait(total_processes);  //decrease number of people to init
            sem_post(SOI_init);         //unlock for next 
            r->work();
            delete r;
        }
    }


    close_sem();
    destroySems();
    destroyQ();
    return 0;
}


void close_sem()
{
    sem_close(total_processes);
    sem_close(SOI_init);

    sem_unlink("/soi_total_processes");
    sem_unlink("/soi_init");

}
