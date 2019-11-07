
/** @file client.c
 *  @brief The client.c implements the two players in the game
 *
 *  This program implement the client function which represents a player of the game.
 *  It cretes two threads and uses semaphores to make sure that the players are playing 
 *  in turns, and only one player accesses the shared memory.
 *
 *  @author Marios Pafitis
 *  @bug No known bugs.
 *  
 */
#include "game.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

sem_t sem;

/** @brief The client function for the game
 *
 *  This function represents a player. It uses the shared memory that the server had already
 *  generated which represents the board to find a cell to flip. When a player finds a cell, 
 *  it replaces the values 'o' which represents the ball with its number. The player that finds
 *  the last ball wins. If the player doesn't find a ball and the cell is 'x' it replaces the 
 *  cell with the character '+' to indicate that the cell is already fliped. The cells are 
 *  randomly selected.  
 *
 *  @return 
 *
 */
void *client()
{
    int size = SIZE * SIZE + EXTRA;
    int boardSize = SIZE * SIZE;
    int turn = size - 3;
    int flag = size - 2;

    int r, shmid;
    char *shm, *s, c;

    /* Use current time as seed for random generator */
    srand(time(0));
    /* Key for the shared memory */
    key_t key = ftok("shmfile", 65);

    /* Locate the segment. */
    if ((shmid = shmget(key, size, 0666)) < 0)
    {
        perror("shmget in client");
        exit(1);
    }

    /* Now we attach the segment to our data space. */
    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat in client");
        exit(1);
    }

    /* Wait the server to create the board*/
    while (shm[flag] != 'r')
    {
        sleep(SLEEP);
    }

    while (1)
    {
        /* Is my turn?*/
        sem_wait(&sem);

        if (shm[turn] == '0')
        {
            shm[turn] = '1';
        }
        else
        {
            shm[turn] = '0';
        }

        /* if the response is f it means the game is finished and I stop*/
        if (shm[flag] == 'f')
        {
            c = shm[turn];
            printf("Player %c exits the game.\n", c);
            sem_post(&sem);
            break;
        }

        /* Make a move*/
        printf("Player %c makes a move.\n", shm[turn]);

        s = shm;
        r = rand() % (boardSize);
        while (s[r] == '1' || s[r] == '0' || s[r] == '+')
        {
            r = rand() % (boardSize);
        }
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
        /* Now is your turn */
        sem_post(&sem);
    }

    /* detach from shared memory */
    shmdt(shm);
    printf("Player %c returns.\n", c);
    pthread_exit(0);
}

/** @brief The main for server.c
 *
 *  The main function creates two threads that represent the two players, and each one is uses 
 *  the function client. The use a semaphore to control the turns and when the game is finished 
 *  we destroy the threads and the semaphore. 
 *
 *  @param int argc number of arguments given to main
 *  @param char *argv[] arguments passed to main
 *
 *  @return 0
 *
 */
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
