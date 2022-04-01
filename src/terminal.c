/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Display Handler Header for ASCII Terminal I/O.
 */

/*----------------------------------------------------------------------
 * Includes.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "display.h"
#include "game.h"
#include "barony.h"
#include "report.h"
#include "order.h"
#include "attack.h"

/*----------------------------------------------------------------------
* Data Definitions.
 */

/** @var control_names are the names of who can control baronies. */
static char *control_names[] = {
    "Human",
    "Computer",
    "Remote"
};

/** @var player is the player currently in control */
static int player;

/** @var viewed is the barony currently viewed */
static int viewed;

/** @var humans is the number of human players */
static int humans;

/*----------------------------------------------------------------------
 * Private Level 1 Functions.
 */

/**
 * Display the details of a barony.
 * @param barony is the barony to display.
 * @param turn is the game turn.
 */
static void display_barony (barony_t *barony, int turn)
{
    printf ("Barony: %s   ranking: %d   turn: %d\n",
	    barony->name, barony->ranking, turn + 1);
    printf ("   land: %d   population: %d   gold: %d\n",
	    barony->land, barony->population, barony->gold);
    printf ("   castles: %d   knights: %d   footmen: %d\n",
	    barony->castles, barony->knights, barony->footmen);
}

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Initialise the display handler.
 */
state_t display_open (int argc, char **argv)
{
    return DISPLAY_NEW_GAME;
}

/**
 * Handle the New Game screen.
 * @param game is the game to configure.
 */
state_t display_new_game (game_t *game)
{
    int b, /* barony counter */
	input; /* option input */
    char text_input[81]; /* text input */

    /* print title */
    printf ("NEW GAME:\n");
    
    /* display the baronies */
    for (b = 0; b < BARONIES; ++b)
	printf ("%d: %s - %s\n", b + 1, game->baronies[b]->name,
		control_names[game->baronies[b]->control]);
    printf ("0: start game\n");
    humans = 0;

    /* main input loop */
    do {

	/* display option and get input */
	printf ("Option: ");
	scanf ("%s", text_input);
	input = atoi (text_input);

	/* switch player between human and computer */
	if (input > 0 && input <= BARONIES) {
	    b = input - 1;
	    if (game->baronies[b]->control == CONTROL_HUMAN) {
		game->baronies[b]->control = CONTROL_COMPUTER;
		humans--;
	    } else {
		game->baronies[b]->control = CONTROL_HUMAN;
		humans++;
		if (humans == 1)
		    player = viewed = b;
	    }
	    printf ("%d: %s - %s\n", b + 1, game->baronies[b]->name,
		    control_names[game->baronies[b]->control]);
	}

	/* warn if starting game without humans */
	else if (input == 0 && humans == 0)
	    printf ("Select at least 1 human player first!\n");

    } while (input || ! humans);
    printf ("\n");

    /* return the next display state */
    return DISPLAY_INTRODUCTION;
}

/**
 * Hand the Introduction screen.
 * @param game is the game to start.
 * @return the new display state.
 */
state_t display_introduction (game_t *game)
{
    printf ("The game is starting!\n\n");
    return humans == 1 ?
	DISPLAY_OWN_BARONY :
	DISPLAY_MAIN_MENU;
}

/**
 * Handle the Main Menu screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_main_menu (game_t *game)
{
    int b, /* barony counter */
	input; /* option input */
    char text_input[81]; /* text input */

    /* display the title */
    printf ("MAIN MENU:\n");
    printf ("Turn %d\n", game->turn + 1);

    /* display the baronies */
    for (b = 0; b < BARONIES; ++b)
	if (game->baronies[b]->control == CONTROL_HUMAN)
	    printf ("%d: %s - %s\n", b + 1, game->baronies[b]->name,
		    control_names[game->baronies[b]->control]);
	else
	    printf ("   %s - %s\n", game->baronies[b]->name,
		    control_names[game->baronies[b]->control]);
    printf ("0: end turn\n");

    /* input barony */
    do {
	printf ("Option: ");
	scanf ("%s", text_input);
	input = atoi (text_input);
	if (input > 0 && input <= BARONIES &&
	    game->baronies[input - 1]->control == CONTROL_HUMAN)
	    viewed = player = input - 1;
    } while (input < 0 || input > BARONIES ||
	     (input > 0 &&
	      game->baronies[input - 1]->control != CONTROL_HUMAN));
    printf ("\n");

    /* return the appropriate display state */
    return input
	? DISPLAY_OWN_BARONY
	: DISPLAY_END_TURN;
}

/**
 * Handle the Own Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_own_barony (game_t *game)
{
    int input; /* option input */
    char text_input[81]; /* text input */
    
    /* display the barony and the menu */
    display_barony (game->baronies[viewed], game->turn);
    printf ("1: Spend   2: Report   3: Previous   4: Next   0: Done\n");

    /* menu loop */
    do {
	printf ("Option: ");
	scanf ("%s", text_input);
	input = atoi (text_input);
	switch (input) {
	case 0:
	    printf ("\n");
	    return humans == 1 ?
		DISPLAY_END_TURN :
		DISPLAY_MAIN_MENU;
	case 1:
	    printf ("\n");
	    return DISPLAY_ORDER;
	case 2:
	    printf ("\n");
	    return DISPLAY_REPORT;
	case 3:
	case 4:
	    printf ("\n");
	    viewed += 2 * input - 7;
	    viewed &= 7;
	    return DISPLAY_OTHER_BARONY;
	}
    } while (1);
}

/**
 * Handle the Other Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_other_barony (game_t *game)
{
    int input; /* option input */
    char text_input[81]; /* text input */
    
    /* display the barony and the menu */
    display_barony (game->baronies[viewed], game->turn);
    printf ("1: Attack   3: Previous   4: Next   0: Done\n");

    /* menu loop */
    do {
	printf ("Option: ");
	scanf ("%s", text_input);
	input = atoi (text_input);
	switch (input) {
	case 0:
	    printf ("\n");
	    return humans == 1 ?
		DISPLAY_END_TURN :
		DISPLAY_MAIN_MENU;
	case 1:
	    printf ("\n");
	    return DISPLAY_ATTACK;
	case 3:
	case 4:
	    printf ("\n");
	    viewed += 2 * input - 7;
	    viewed &= 7;
	    return viewed == player ?
		DISPLAY_OWN_BARONY :
		DISPLAY_OTHER_BARONY;
	}
    } while (1);
}

/**
 * Handle the Order screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_order (game_t *game)
{
    int castles, /* castles to build */
	knights, /* knights to train */
	footmen, /* footmen to draft */
	max; /* maximum number of units */
    char text_input[81]; /* text input */

    /* clear out any existing order */
    if (game->orders[player])
	free (game->orders[player]);
    game->orders[player] = new_order (game->baronies[player]);

    /* input castles */
    max = max_castles_to_buy (game, player);
    do {
	printf ("Castles (0..%d): ", max);
	scanf ("%s", text_input);
	castles = atoi (text_input);
    } while (castles < 0 || castles > max);
    game->orders[player]->castles = castles;

    /* input knights */
    max = max_knights_to_buy (game, player);
    do {
	printf ("Knights (0..%d): ", max);
	scanf ("%s", text_input);
	knights = atoi (text_input);
    } while (knights < 0 || knights > max);
    game->orders[player]->knights = knights;

    /* input footmen */
    max = max_footmen_to_buy (game, player);
    do {
	printf ("Footmen (0..%d): ", max);
	scanf ("%s", text_input);
	footmen = atoi (text_input);
    } while (footmen < 0 || footmen > max);
    game->orders[player]->footmen = footmen;

    /* remove the order if no units were ordered */
    if (castles + knights + footmen == 0) {
	free (game->orders[player]);
	game->orders[player] = NULL;
    }

    /* return to own barony view */
    printf ("\n");
    return DISPLAY_OWN_BARONY;
}

/**
 * Handle the Attack Screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_attack (game_t *game)
{
    int knights, /* knights to send */
	footmen, /* footmen to send */
	max; /* maximum force available */
    char text_input[81]; /* text input */

    /* make sure there is an attack to edit/display */
    if (! game->attacks[player][viewed])
	game->attacks[player][viewed] =
	    new_attack (game->baronies[player],
			game->baronies[viewed],
			0, 0);

    /* input knights */
    max = max_knights_to_send (game, player, viewed);
    do {
	printf ("Knights (0..%d): ", max);
	scanf ("%s", text_input);
	knights = atoi (text_input);
    } while (knights < 0 || knights > max);
    game->attacks[player][viewed]->knights = knights;

    /* input footmen */
    max = max_footmen_to_send (game, player, viewed);
    do {
	printf ("Footmen (0..%d): ", max);
	scanf ("%s", text_input);
	footmen = atoi (text_input);
    } while (footmen < 0 || footmen > max);
    game->attacks[player][viewed]->footmen = footmen;

    /* get rid of attacks with no troops */
    if (game->attacks[player][viewed]->knights +
	game->attacks[player][viewed]->footmen == 0) {
	free (game->attacks[player][viewed]);
	game->attacks[player][viewed] = NULL;
    }

    /* return next state */
    printf ("\n");
    return DISPLAY_OTHER_BARONY;
}

/**
 * Display a Barony Report.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_report (game_t *game)
{
    int b; /* barony counter */
    report_t *report; /* the barony's report */

    /* print the report header */
    printf ("Report on %s barony.\n", game->baronies[player]->name);
    
    /* return if there is no report */
    if (! (report = game->reports[player])) {
	printf ("Nothing to report!\n\n");
	return DISPLAY_OWN_BARONY;
    }

    /* report on attacks sent out */
    for (b = 0; b < BARONIES; ++b)
	if (report->attacks[b]) {
	    printf ("You attacked the barony of %s",
		    report->attacks[b]->target->name);
	    printf (" with %d knights and %d footmen.\n",
		    report->attacks[b]->knights_sent,
		    report->attacks[b]->footmen_sent);
	    printf ("You took %d land", report->attacks[b]->land_taken);
	    printf (" and looted %d gold.\n",
		    report->attacks[b]->gold_looted);
	    printf ("You took possession of %d castles,",
		    report->attacks[b]->castles_taken);
	    printf (" and razed %d,",
		    report->attacks[b]->castles_razed);
	    printf (" and slew %d footmen.\n",
		    report->attacks[b]->footmen_slain);
	    printf ("You lost %d knights",
		    report->attacks[b]->knights_lost);
	    printf (" and %d footmen in the attack.\n",
		    report->attacks[b]->footmen_lost);
	}

    /* report on defence against attacks sent against us */
    for (b = 0; b < BARONIES; ++b)
	if (report->defences[b]) {
	    printf ("You were attacked by the barony of %s",
		    report->defences[b]->origin->name);
	    printf (" with %d knights and %d footmen.\n",
		    report->defences[b]->knights_sent,
		    report->defences[b]->footmen_sent);
	    printf ("They took %d land",
		    report->defences[b]->land_taken);
	    printf (" and looted %d gold.\n",
		    report->defences[b]->gold_looted);
	    printf ("They took possession of %d castles,",
		    report->defences[b]->castles_taken);
	    printf (" and razed %d,",
		    report->defences[b]->castles_razed);
	    printf (" and slew %d footmen.\n",
		    report->defences[b]->footmen_slain);
	    printf ("You slew %d knights",
		    report->defences[b]->knights_lost);
	    printf (" and %d footmen in the attack.\n",
		    report->defences[b]->footmen_lost);
	}

    /* population migration and tax income */
    if (report->migration > 0)
	printf ("%d people settled in your barony.\n",
		report->migration);
    else if (report->migration < 0)
	printf ("%d people fled from your barony.\n",
		-report->migration);
    printf ("Your people paid %d gold in tax.\n", report->tax);

    /* new forces */
    if (report->delivered) {
	printf ("You built %d castles,", report->delivered->castles);
	printf (" trained %d knights,", report->delivered->knights);
	printf (" and drafted %d footmen.\n",
		report->delivered->footmen);
    }

    /* unaffordable forces */
    if (report->notdelivered) {
	printf ("You were unable to afford the %d castles,",
		report->notdelivered->castles);
	printf (" %d knights,", report->notdelivered->knights);
	printf (" and %d footmen you asked for.\n",
		report->notdelivered->footmen);
    }

    /* desertion */
    if (report->attrition) {
	printf ("Due to lack of funds, %d castles fell into ruin,",
		report->attrition->castles);
	printf (" %d knights deserted your cause,",
		report->attrition->knights);
	printf (" and %d footmen died of starvation.\n",
		report->attrition->footmen);
    }

    /* outgoings */
    printf ("You paid %d gold for the upkeep of",
	    report->barony->castles
	    + report->barony->knights / 10
	    + report->barony->footmen / 50);
    printf (" %d castles,", report->barony->castles);
    printf (" %d knights,", report->barony->knights);
    printf (" and %d footmen.\n", report->barony->footmen);

    /* return next state */
    printf ("\n");
    return DISPLAY_OWN_BARONY;
}

/**
 * Handle the End Turn screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_turn (game_t *game)
{
    int b; /* barony counter */

    /* display wait message */
    printf ("Turn %d is being processed...\n\n", game->turn + 1);

    /* if one human player, go straight to their barony afterwards */
    if (humans == 1)
	for (b = 0; b < BARONIES; ++b)
	    if (game->baronies[b]->control == CONTROL_HUMAN) {
		player = viewed = b;
		return game->turn == 11 ?
		    DISPLAY_END_GAME :
		    DISPLAY_OWN_BARONY;
	    }

    /* otherwise go to the main menu afterwards */
    return game->turn == 11 ?
	DISPLAY_END_GAME :
	DISPLAY_MAIN_MENU;
}

/**
 * Handle the End Game screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_game (game_t *game)
{
    int l, /* last rank displayed */
	r, /* ranking counter */
	b, /* barony counter */
	input; /* user input */
    char text_input[81]; /* text input */

    /* title */
    printf ("END GAME\n");
    /* show the baronies in rank order */
    l = 0;
    for (r = 1; r <= BARONIES; ++r)
	for (b = 0; b < BARONIES; ++b)
	    if (game->baronies[b]->ranking == r) {
		if (l == r)
		    printf ("   %s (key %d)\n",
			    game->baronies[b]->name, b + 1);
		else
		    printf ("%d  %s (key %d)\n", r,
			    game->baronies[b]->name, b + 1);
		l = r;
	    }
    printf ("Type 0 to quit\n");

    /* allow inspection of a barony */
    do {
	printf ("Option: ");
	scanf ("%s", text_input);
	input = atoi (text_input);
	if (input == 0) {
	    printf ("\n");
	    return DISPLAY_QUIT;
	} else if (input > 0 && input <= BARONIES) {
	    printf ("\n");
	    viewed = input - 1;
	    return DISPLAY_END_BARONY;
	}
    } while (1);
}

/**
 * Handle the End Barony screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_barony (game_t *game)
{
    display_barony (game->baronies[viewed], 11);
    printf ("\n");
    return DISPLAY_END_GAME;
}

/**
 * Clean up the display handler.
 */
void display_close (void)
{
}
