/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Display Handler Header.
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* typedefs */
typedef struct game game_t;
typedef struct barony barony_t;
typedef struct report report_t;

/* @enum state_t is the display state */
typedef enum {
    DISPLAY_NEW_GAME,
    DISPLAY_INTRODUCTION,
    DISPLAY_MAIN_MENU,
    DISPLAY_OWN_BARONY,
    DISPLAY_OTHER_BARONY,
    DISPLAY_ORDER,
    DISPLAY_ATTACK,
    DISPLAY_REPORT,
    DISPLAY_END_TURN,
    DISPLAY_END_GAME,
    DISPLAY_END_BARONY,
    DISPLAY_QUIT
} state_t;

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Initialise the display handler.
 * @param argc is the number of command line arguments.
 * @param argv is an array of command line arguments.
 */
state_t display_open (int argc, char **argv);

/**
 * Handle the New Game screen.
 * @param game is the game to configure.
 * @return the new display state.
 */
state_t display_new_game (game_t *game);

/**
 * Hand the Introduction screen.
 * @param game is the game to start.
 * @return the new display state.
 */
state_t display_introduction (game_t *game);

/**
 * Handle the Main Menu screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_main_menu (game_t *game);

/**
 * Handle the Own Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_own_barony (game_t *game);

/**
 * Handle the Barony Selector screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_select_barony (game_t *game);

/**
 * Handle the Other Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_other_barony (game_t *game);

/**
 * Handle the Order screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_order (game_t *game);

/**
 * Handle the Attack Screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_attack (game_t *game);

/**
 * Display a Barony Report.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_report (game_t *game);

/**
 * Handle the End Turn screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_turn (game_t *game);

/**
 * Handle the End Game screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_game (game_t *game);

/**
 * Handle the End Barony screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_barony (game_t *game);

/**
 * Clean up the display handler.
 */
void display_close (void);

#endif
