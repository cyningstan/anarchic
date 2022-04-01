/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Main Module.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "anarchic.h"
#include "game.h"
#include "barony.h"
#include "display.h"
#include "attack.h"
#include "order.h"
#include "computer.h"
#include "beta.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* pointer to game in play. */
static game_t *game;

/* display state */
static state_t state = DISPLAY_NEW_GAME;

/*----------------------------------------------------------------------
 * Level 1 Routines.
 */

/**
 * Initialise the program.
 */
static void initialise (int argc, char **argv)
{
    /* beta_check (); */
    state = display_open (argc, argv);
    game = new_game ();
}

/**
 * Play a single game.
 * @return 1 to play again, 0 to finish.
 */
static int play (void)
{
    switch (state) {
    case DISPLAY_NEW_GAME:
	state = display_new_game (game);
	break;
    case DISPLAY_INTRODUCTION:
	state = display_introduction (game);
	break;
    case DISPLAY_MAIN_MENU:
	state = display_main_menu (game);
	break;
    case DISPLAY_OWN_BARONY:
	state = display_own_barony (game);
	break;
    case DISPLAY_OTHER_BARONY:
	state = display_other_barony (game);
	break;
    case DISPLAY_ORDER:
	state = display_order (game);
	break;
    case DISPLAY_ATTACK:
	state = display_attack (game);
	break;
    case DISPLAY_REPORT:
	state = display_report (game);
	break;
    case DISPLAY_END_TURN:
	state = display_end_turn (game);
	computer_turns (game);
	process_turn (game);
	break;
    case DISPLAY_END_GAME:
	state = display_end_game (game);
	break;
    case DISPLAY_END_BARONY:
	state = display_end_barony (game);
	break;
    default:
	state = DISPLAY_QUIT;
    }
    return state != DISPLAY_QUIT;
}

/**
 * Clean up after the program.
 */
static void cleanup (void)
{    
    end_game (game);
    display_close ();
}

/*----------------------------------------------------------------------
 * Top Level Routine.
 */

/**
 * Main function.
 * @param argc is the number of command line arguments.
 * @param argv is an array of command line arguments.
 * @return 0 if successful, >0 on error.
 */
int main (int argc, char **argv) {
    initialise (argc, argv);
    while (play ());
    cleanup ();
    return 0;
}
