
/** @file server.c
 *  @brief The server.c is the game server of the game
 *
 *  This program allocates space in shared memory and generates a board that contains a number of ball. 
 *  Then two players can access the shared memory and play in turns until the game is finished, so
 *  one of the two players had found the last ball. Each time the two payers select a ball, the server
 *  checks whether the game is finished. The server and the clients communicate only through the shared memory.
 *  
 *  @author Marios Pafitis
 *  @bug No known bugs.
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

/** @brief The server
 *
 *  This is the server function. The server is responsible to allocate the shared memory and assign 
 *  the board in it. It randomly generates a board of size SIZE * SIZE which can be accessed and 
 *  modified in the file game.h. The server reserver 3 extra bits in the shared memory. The first bit
 *  is a flag to recognise whether it is his turn to operate and to inform the players that the game
 *  is finished if it is. The second bit is to identify which player is playing. When the game is 
 *  finished he frees the shared memory.
 *
 *
 *  @return 
 *
 */
void server()
{
    int size = SIZE * SIZE + EXTRA;
    int boardSize = SIZE * SIZE;
    int turn = size - 3;
    int flag = size - 2;

    /* Create the board */
    int i, j, r, finish, balls, shmid, isSolved;
    char *shm, *s;

    /* Check if the input is legal */
    if (SIZE < 2 || BALLS < 1 || BALLS > boardSize)
    {
        printf("Illegal input: SIZE should be greater than 1. Balls should be greter than 0 and less than the SIZE * SIZE\n");
        return;
    }
    /* Use current time as seed for random generator */
    srand(time(0));

    /* ftok to generate unique key */
    key_t key = ftok("shmfile", 65);

    /* Create the segment. */
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget server");
        exit(1);
    }

    /* Now we attach the segment to our data space. */
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

    /* Print the board */
    printf("Board created!\n");
    s = shm;
    for (i = 0; i < boardSize; i++)
    {
        if (i % SIZE == 0)
        {
            printf("\n");
            for (j = 0; j < SIZE; j++)
            {
                printf("+---");
            }
            printf("+\n");
            printf("| ");
        }
        printf("%c | ", s[i]);
    }
    printf("\n");
    for (j = 0; j < SIZE; j++)
    {
        printf("+---");
    }
    printf("+\n\n");

    /*Inform player 1 and player 2 that the board is ready */
    shm[turn] = '0';
    shm[flag] = 'r';

    while (shm[flag] == 'r')
    {
        sleep(SLEEP);
    }
    finish = 0;
    while (finish == 0)
    {
        /* Check if the board is solved and print the puzzle */

        printf("Player %c\n", shm[turn]);
        isSolved = 1;
        s = shm;
        for (i = 0; i < boardSize; i++)
        {
            if (i % SIZE == 0)
            {
                printf("\n");
                for (j = 0; j < SIZE; j++)
                {
                    printf("+---");
                }
                printf("+\n");
                printf("| ");
            }
            printf("%c | ", s[i]);
            if (s[i] == 'o')
            {
                isSolved = 0;
            }
        }
        printf("\n");
        for (j = 0; j < SIZE; j++)
        {
            printf("+---");
        }
        printf("+\n\n");

        /* If it is solved announce the winner and stop the game*/
        if (isSolved == 1)
        {
            /* Display the winner */
            printf("The winner is Player %c\n", shm[turn]);
            shm[flag] = 'f';
            finish = 1;
        }
        else
        {
            shm[flag] = 'c';
        }
        while (isSolved == 0 && shm[flag] != '*')
        {
            /* You can remove this sleep value */
            sleep(SLEEP);
        }
    }
    /* Wait for the clients to exit the game */
    printf("Game Over.\n");

    sleep(1);
    /* detach from shared memory */
    shmdt(shm);
    /* Remove shared memory */
    shmctl(shmid, IPC_RMID, NULL);
}

/** @brief The main for server.c
 *
 *  This is the main function of the server.c It doesn't have any special functionality, it just calls the server function.
 *
 *  @return 0
 *
 */
int main()
{
    server();
    return 0;
}
