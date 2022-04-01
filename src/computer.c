/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Computer Player Module.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* project headers */
#include "computer.h"
#include "anarchic.h"
#include "game.h"
#include "barony.h"
#include "attack.h"
#include "order.h"

/*----------------------------------------------------------------------
 * Private Level 1 Functions.
 */

/**
 * Play a turn for a single computer-controlled barony.
 * @param game is the game in play.
 * @param player is the barony to play.
 */
static void computer_turn (game_t *game, int player)
{
    int target; /* target for attack */
    
    /* make a random attack */
    target = rand () % BARONIES;
    if (target != player)
	game->attacks[player][target] =
	    new_attack (game->baronies[player],
			game->baronies[target],
			game->baronies[player]->knights / 2,
			game->baronies[player]->footmen / 2);

    /* make a purchase */
    game->orders[player] = new_order (game->baronies[player]);
    game->orders[player]->castles = game->baronies[player]->gold / 400;
    game->orders[player]->knights = game->baronies[player]->gold / 40;
    game->orders[player]->footmen = game->baronies[player]->gold / 8;
}

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Play the computer turns.
 * @param game is the game in play.
 */
void computer_turns (game_t *game)
{
    int b; /* barony counter */

    /* randomise seed at the start of the game */
    if (game->turn == 1)
	srand (time (0));
    
    /* loop through all the computer baronies */
    for (b = 0; b < BARONIES; ++b)
	if (game->baronies[b]->control == CONTROL_COMPUTER)
	    computer_turn (game, b);
}
