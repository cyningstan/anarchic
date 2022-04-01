/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Computer Player Header.
 */

#ifndef __COMPUTER_H__
#define __COMPUTER_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* typedefs */
typedef struct game game_t;

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Play the computer turns.
 * @param game is the game in play.
 */
void computer_turns (game_t *game);

#endif
