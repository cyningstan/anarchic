/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Game Header.
 */

#ifndef __GAME_H__
#define __GAME_H__

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* main project header */
#include "anarchic.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* type definitions */
typedef struct game game_t;
typedef struct barony barony_t;
typedef struct attack attack_t;
typedef struct order order_t;
typedef struct report report_t;

/** @struct game is the data for a single game. */
struct game {

    /** @var turn is the turn number. */
    int turn;

    /** @var baronies are the baronies in the game. */
    barony_t *baronies[BARONIES];

    /** @var attacks are the attacks for a turn. */
    attack_t *attacks[BARONIES][BARONIES];

    /** @var orders are the build/recruit orders for a turn. */
    order_t *orders[BARONIES];

    /** @var reports are the reports for each barony. */
    report_t *reports[BARONIES];

};

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Create and initialise a new game.
 * @return the new game.
 */
game_t *new_game (void);

/**
 * Process a single turn.
 * @param game is the game to process.
 */
void process_turn (game_t *game);

/**
 * Clean up after a game.
 * @param game is the game to clean up.
 */
void end_game (game_t *game);

/**
 * Work out the maximum number of castles affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_castles_to_buy (game_t *game, int b);

/**
 * Work out the maximum number of knights affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_knights_to_buy (game_t *game, int b);

/**
 * Work out the maximum number of footmen affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_footmen_to_buy (game_t *game, int b);

/**
 * Work out the number of knights available for attack.
 * @param game is the game in progress.
 * @param o is the barony originating the attack.
 * @param t is the target barony.
 */
int max_knights_to_send (game_t *game, int o, int t);

/**
 * Work out the number of footmen available for attack.
 * @param game is the game in progress.
 * @param o is the barony originating the attack.
 * @param t is the target barony.
 */
int max_footmen_to_send (game_t *game, int o, int t);

/**
 * Work out the expenses for a barony's military.
 * @param barony is the barony concerned.
 */
unsigned long int calculate_expenses (barony_t *barony);

#endif
