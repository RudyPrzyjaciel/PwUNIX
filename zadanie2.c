#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>

#define N_GOODS 40

struct shared_memory
{
    int PRODUCTS_AV[N_GOODS];
    int PRODUCED[N_GOODS];
    int CONSUMED[N_GOODS];
    pthread_mutex_t my_mutex;
};


int main()
{
    int N_PRODUCERS = 10;
    int BATCH_SIZE = 10;
    int PRODUCTION_RUN = 25000;
    int N_CUSTOMERS = 20;
    int CONSUMPTION_RUN = 100000;
    int product;
    int goods;

    int shmid = shmget(2540362, sizeof(struct shared_memory), IPC_CREAT|0600);
    struct shared_memory* memory;
    if(shmid < 0)
    {
        printf("ERROR DURING SHMID!\n");
        return 0;
    }
    memory = shmat(shmid, 0, 0);

    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&memory->my_mutex, &attr);

    for(int i = 0; i < N_GOODS;++i)
    {
        memory->PRODUCED[i] = 0;
        memory->PRODUCTS_AV[i] = 0;
        memory->CONSUMED[i]=0;
    }

    for(int i = 0; i<N_PRODUCERS; i++)
    {
        if(fork() == 0)
        {
            srand(getpid());
            for(int j = 0; j < PRODUCTION_RUN; ++j)
            {
                for(int k = 0; k < BATCH_SIZE; ++k)
                {
                    product = rand() % N_GOODS;
                    
			        pthread_mutex_lock(&memory->my_mutex);
                    memory->PRODUCED[product]++;
                    memory->PRODUCTS_AV[product]++;
			        pthread_mutex_unlock(&memory->my_mutex);
                }
            }
            exit(0);
        }
    }

    for(int i = 0; i < N_CUSTOMERS; ++i)
    {
        if(fork() == 0)
        {
            srand(getpid());
            for(int j = 0; j < CONSUMPTION_RUN; ++j)
            {
                goods = rand() % N_GOODS;
                pthread_mutex_lock(&memory->my_mutex);
                if(memory->PRODUCTS_AV[goods]>0)
                {
                    memory->PRODUCTS_AV[goods]--;
                    memory->CONSUMED[goods]++;
                }
                else
                {
                    j--;
                }
                pthread_mutex_unlock(&memory->my_mutex);
            }
            exit(0);
        }
    }

    for(int i = 0; i < N_PRODUCERS; ++i)
    {
        wait(NULL);
    }

    for(int i = 0; i < N_CUSTOMERS; ++i)
    {
        wait(NULL);
    }


    printf("INDEX\t|PRODUCED\t|CONSUMED\t|REMAINS\t|BALANCE STATUS\n");
    for (int i = 0; i < N_GOODS; ++i)
    {
        printf("Product [#%d]|%d\t|%d\t\t|%d", i, memory->PRODUCED[i], memory->CONSUMED[i], memory->PRODUCTS_AV[i]);
        if ((memory->PRODUCED[i] - memory->CONSUMED[i]) != memory->PRODUCTS_AV[i])
        {
            printf("\t\t[incorrect]\n");
        }
        else
        {
            printf("\t\t[correct]\n");
        }
    }

    int sum_produced = 0;
    int sum_consumed = 0;
    int sum_remain = 0;
    for (int i = 0; i < N_GOODS; ++i)
    {
        sum_produced += memory->PRODUCED[i];
        sum_consumed += memory->CONSUMED[i];
        sum_remain += memory->PRODUCTS_AV[i];
    }
    printf("\n-----\nIn total Produced: %d, Consumed: %d and Remains: %d goods\n", sum_produced, sum_consumed, sum_remain);
    if ((sum_produced - sum_consumed) != sum_remain)
    {
        printf("Balance is incorrect!\n");
    }
    else
    {
        printf("Overall balance is correct!\n");
    }
    return 0;
}
