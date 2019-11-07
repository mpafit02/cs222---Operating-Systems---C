/** @file game.h 
 *  @brief This is the header file for the game.
 * 
 */

#ifndef GAME_H
#define GAME_H

/** @brief Board Size
 *  The board size will be size of SIZE * SIZE so for SIZE = 4 -> 4 x 4 = 16 , (SIZE >= 2)
 */
#define SIZE 4

/** @brief Extra Soace fir allocation, DO NOT MODIFY!
 *  1 for the turns plus 1 for the client/server/finish plus 1 for the nul
 */
#define EXTRA 3

/** @brief Number of Balls
 *  Number of balls in the game
 */
#define BALLS 4

/** @brief Sleep time between turns of the players
 *  Sleep time for presentation purposes, you can set it to '1' if you would like to see the PLayers playing
 */
#define SLEEP 0

#endif
