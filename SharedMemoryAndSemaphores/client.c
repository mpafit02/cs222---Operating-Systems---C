
#include "game.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>

int size = SIZE * SIZE + EXTRA;
int boardSize = SIZE * SIZE;
int turn = size - 3;
int flag = size - 2;
sem_t sem;

void *client()
{
    int r, shmid, finish;
    char *shm, *s;

    /* Use current time as seed for random generator */
    srand(time(0));
    /* ftok to generate unique key */
    key_t key = ftok("shmfile", KEY);
    /*
   * Locate the segment.
   */
    if ((shmid = shmget(key, size, 0666 | IPC_CREAT)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    /*
   * Now we attach the segment to our data space.
   */
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    /* Wait the server to create the board*/
    while (shm[flag] != 'r')
    {
        sleep(1);
    }
    finish = 0;
    while (finish == 0)
    {
        /* Is my turn?*/
        sem_wait(&sem);
        /* if the response is f it means the game is finished and I stop*/
        if (shm[flag] == 'f')
        {
            finish = 1;
        }

        if (shm[turn] == '0')
        {
            shm[turn] = '1';
        }
        else
        {
            shm[turn] = '0';
        }

        /* Make a move*/
        printf("Player %c makes a move!\n", shm[turn]);

        r = rand() % (boardSize);
        while (s[r] == '1' || s[r] == '+')
        {
            r = rand() % (boardSize);
        }
        s = shm;
        if (s[r] == 'x')
        {
            s[r] = '+';
        }
        else if (s[r] == 'o')
        {
            s[r] = shm[turn];
        }

        /* Server check if I won*/
        shm[flag] = '*';
        /* Wait for the Server to respond*/
        while (shm[flag] == '*')
        {
            sleep(SLEEP);
        }

        /* if the response is f it means the game is finished and I stop*/
        if (shm[flag] == 'f')
        {
            finish = 1;
        }
        /* Now is your turn */
        sem_post(&sem);
    }
}

int main(int argc, char *argv[])
{
    /* Create the semaphore */
    sem_init(&sem, 0, 1);

    pthread_t t1;
    pthread_t t2;

    /* Create the threads */
    pthread_create(&t1, NULL, client, NULL);
    pthread_create(&t2, NULL, client, NULL);
    /*
   * If the main thread returns, the process terminates even if there are running 
   * threads in that process, unless special precautions are taken.
   *
   * Threads should always be joined; otherwise, a thread might keep on running even
   * when the main thread has already terminated.
  */

    pthread_join(t1, NULL); /*Wait for the thread to finish before exiting*/
    pthread_join(t2, NULL);

    /* Destroy the semaphore */
    sem_destroy(&sem);
    return 0;
}
