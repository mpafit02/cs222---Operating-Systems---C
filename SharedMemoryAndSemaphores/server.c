
#include "game.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <time.h>

void server()
{
    int size = SIZE * SIZE + EXTRA;
    int boardSize = SIZE * SIZE;
    int turn = size - 3;
    int flag = size - 2;

    /* Create the board */
    char c;
    int i, r, finish, balls, shmid, isSolved;
    char *shm, *s;

    /* Use current time as seed for random generator */
    srand(time(0));
    /* ftok to generate unique key */
    key_t key = ftok("shmfile", KEY);

    /*
   * Create the segment.
   */
    if ((shmid = shmget(key, SIZE, IPC_CREAT | 0666)) < 0)
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

    /* Create the board */
    s = shm;
    for (i = 0; i < boardSize; i++)
    {
        s[i] = 'x';
    }
    s[i] = '\0';

    /* Randomly place the balls in the board */
    balls = 0;
    s = shm;
    while (balls != BALLS)
    {
        r = rand() % (boardSize);
        if (s[r] == 'x')
        {
            s[r] = 'o';
            balls++;
        }
    }

    /*Inform player 1 and player 2 that the board is ready */
    shm[turn] = '0';
    shm[flag] = 'r';

    while (shm[flag] == 'r')
    {
        sleep(SLEEP);
    }
    finish = 0;
    while (!finish)
    {
        /* Check if the board is solved*/

        printf("Player %c\n", shm[turn]);
        isSolved = 1;
        s = shm;
        for (i = 0; i < boardSize; i++)
        {
            if (i % SIZE == 0)
            {
                printf("\n");
            }
            printf("%c", s[i]);
            if (s[i] == 'o')
            {
                isSolved = 0;
            }
        }
        printf("\n");
        /* If it is solved announce the winner and stop the game*/
        if (isSolved == 1)
        {
            /* Display the winner */
            finish = 1;
            printf("The winner is Player %c\n", shm[turn]);
            shm[flag] = 'f';
        }
        else
        {
            shm[flag] = 'c';
        }
        while (shm[flag] != '*')
        {
            /* You can remove this sleep value */
            sleep(SLEEP);
        }
    }
}

int main()
{
    server();
    return 0;
}
