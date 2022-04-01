/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Game Module.
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
#include "fatal.h"
#include "barony.h"
#include "attack.h"
#include "order.h"
#include "report.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var names is an array of names for baronies */
static char *names[] = {
    "Beaumont",
    "Cavendish",
    "de la Pole",
    "de Vere",
    "Howard",
    "Neville",
    "Percy",
    "Villiers"
};

/*----------------------------------------------------------------------
 * Level 2 Private Functions.
 */

/**
 * Process inbound attacks on one barony.
 * @param game is the game to process.
 * @param t is the target barony.
 */
static void inbound_attacks (game_t *game, int t)
{
    /* local variables */
    int o; /* origin barony counter */
    unsigned long int
	knights, /* total attacking knights */
	footmen, /* total attacking footmen */
	attack, /* total attacking force value */
	defence, /* total defending force value */
	land_taken, /* amount of land taken in the attack */
	gold_looted, /* amount of gold looted in the attack */
	castles_beaten, /* number of castles taken or razed */
	castles_taken, /* number of castles taken */
	castles_razed, /* number of castles razed */
	knights_lost, /* knights lost by the attackers */
	footmen_lost, /* footmen lost by the attackers */
	footmen_slain; /* defending footmen slain */
    float att_loss, /* proportion of attackers lost */
	def_loss, /* proportion of defenders lost */
	knights_part, /* proportion of knights sent by a barony */
	footmen_part, /* proportion of footmen sent by a barony */
	attack_part; /* proportion of a barony's part of the attack */

    /* get totals of attacking forces and return if none */
    knights = footmen = 0;
    for (o = 0; o < BARONIES; ++o)
	if (game->attacks[o][t]) {
	    knights += game->attacks[o][t]->knights;
	    footmen += game->attacks[o][t]->footmen;
	}
    attack = knights * 10 + footmen;
    if (attack == 0)
	return;

    /* calculate defence */
    defence = game->baronies[t]->castles * 100
	+ game->baronies[t]->footmen;

    /* calculate losses on both sides */
    att_loss = (float) defence / (attack + defence);
    def_loss = (float) attack / (attack + defence);
    land_taken = def_loss * game->baronies[t]->land;
    gold_looted = def_loss * game->baronies[t]->gold;
    castles_beaten = def_loss * game->baronies[t]->castles;
    castles_taken = (knights / (float) (knights + footmen / 10))
	* castles_beaten;
    castles_razed = castles_beaten - castles_taken;
    footmen_slain = def_loss * game->baronies[t]->footmen;
    knights_lost = att_loss * knights;
    footmen_lost = att_loss * footmen;

    /* report on gains and losses to be applied */
    for (o = 0; o < BARONIES; ++o)
	if (game->attacks[o][t]) {
	    
	    /* work out how big a part of the attack was sent */
	    knights_part = knights
		? (float) game->attacks[o][t]->knights / knights
		: 0;
	    footmen_part = footmen
		? (float) game->attacks[o][t]->footmen / footmen
		: 0;
	    attack_part = (float)
		(game->attacks[o][t]->knights * 10
		 + game->attacks[o][t]->footmen) / attack;

	    /* create the attacker battle report */
	    if (! game->reports[o])
		game->reports[o] = new_report (game->baronies[o]);
	    game->reports[o]->attacks[t]
		= malloc (sizeof (attack_report_t));
	    if (! game->reports[o]->attacks[t])
		fatal_error (FATAL_MEMORY);

	    /* create the defender battle report */
	    if (! game->reports[t])
		game->reports[t] = new_report (game->baronies[t]);
	    game->reports[t]->defences[o]
		= malloc (sizeof (attack_report_t));
	    if (! game->reports[t]->defences[o])
		fatal_error (FATAL_MEMORY);

	    /* fill the attack and defence reports simultaneously */
	    game->reports[o]->attacks[t]->origin
		= game->reports[t]->defences[o]->origin
		= game->baronies[o];
	    game->reports[o]->attacks[t]->target
		= game->reports[t]->defences[o]->target
		= game->baronies[t];
	    game->reports[o]->attacks[t]->knights_sent
		= game->reports[t]->defences[o]->knights_sent
		= game->attacks[o][t]->knights;
	    game->reports[o]->attacks[t]->footmen_sent
		= game->reports[t]->defences[o]->footmen_sent
		= game->attacks[o][t]->footmen;
	    game->reports[o]->attacks[t]->land_taken
		= game->reports[t]->defences[o]->land_taken
		= attack_part * land_taken;
	    game->reports[o]->attacks[t]->gold_looted
		= game->reports[t]->defences[o]->gold_looted
		= footmen_part * gold_looted;
	    game->reports[o]->attacks[t]->castles_taken
		= game->reports[t]->defences[o]->castles_taken
		= attack_part * castles_taken;
	    game->reports[o]->attacks[t]->castles_razed
		= game->reports[t]->defences[o]->castles_razed
		= attack_part * castles_razed;
	    game->reports[o]->attacks[t]->footmen_slain
		= game->reports[t]->defences[o]->footmen_slain
		= attack_part * footmen_slain;
	    game->reports[o]->attacks[t]->knights_lost
		= game->reports[t]->defences[o]->knights_lost
		= knights_part * knights_lost;
	    game->reports[o]->attacks[t]->footmen_lost
		= game->reports[t]->defences[o]->footmen_lost
		= footmen_part * footmen_lost;
	}
}

/**
 * Apply the results of a battle to the baronies.
 * @param game is the game in play.
 * @param o is the id of the barony originating the attack.
 * @param t is the id of the target barony of the attack.
 */
static void apply_battle_results (game_t *game, int o, int t)
{
    attack_report_t *report; /* shorthand pointer to attack report */
    barony_t *origin, /* barony originating the attack */
	*target; /* target of the attack */
    attack_t *attack; /* shorthand pointer to the attack */

    /* initialise shorthands */
    report = game->reports[o]->attacks[t];
    origin = game->baronies[o];
    target = game->baronies[t];
    attack = game->attacks[o][t];

    /* transfer land, gold and castle gains */
    origin->land += report->land_taken;
    target->land -= report->land_taken;
    origin->gold += report->gold_looted;
    target->gold -= report->gold_looted;
    origin->castles += report->castles_taken;
    target->castles -= report->castles_taken;

    /* reduce attack forces by what was lost */
    attack->knights -= report->knights_lost;
    attack->footmen -= report->footmen_lost;

    /* apply troop losses to both baronies */
    target->castles -= report->castles_razed;
    origin->knights += report->knights_sent - report->knights_lost;
    origin->footmen += report->footmen_sent - report->footmen_lost;
    target->footmen -= report->footmen_slain;
    
    /* ensure that losses do not result in negative values */
    if (target->land < 0)
	target->land = 0;
    if (target->gold < 0)
	target->gold = 0;
    if (target->castles < 0)
	target->castles = 0;
    if (target->footmen < 0)
	target->footmen = 0;

    /* ensure that gains do not push values outside permitted range */
    if (origin->gold > 32000)
	origin->gold = 32000;
    if (origin->castles > 320)
	origin->castles = 320;
}

/*----------------------------------------------------------------------
 * Level 1 Private Functions.
 */

/**
 * Clear reports from a previous turn.
 * @param game is the game to process.
 */
static void clear_reports (game_t *game)
{
    int b; /* barony counter */
    for (b = 0; b < BARONIES; ++b)
	if (game->reports[b]) {
	    destroy_report (game->reports[b]);
	    game->reports[b] = NULL;
	}
}

/**
 * Take payments for units ordered.
 * @param game is the game to process.
 */
static void take_payments (game_t *game)
{
    /* local variables */
    int b; /* barony counter */
    unsigned long int
	popcost, /* cost of order in population */
	goldcost; /* cost of order in gold */

    /* loop through all the baronies */
    for (b = 0; b < BARONIES; ++b)
	if (game->orders[b]) {
	    popcost = order_popcost (game->orders[b]);
	    goldcost = order_goldcost (game->orders[b]);
	    if (popcost <= game->baronies[b]->population &&
		goldcost <= game->baronies[b]->gold) {
		game->baronies[b]->population -= popcost;
		game->baronies[b]->gold -= goldcost;
		game->orders[b]->paid = 1;
	    }
	}
}

/**
 * Process the attacks sent between the baronies.
 * @param game is the game to process.
 */
static void fight_battles (game_t *game)
{
    /* local variables */
    int b, /* barony counter */
	t; /* target barony counter */

    /* send out all the forces */
    for (b = 0; b < BARONIES; ++b)
	for (t = 0; t < BARONIES; ++t)
	    if (game->attacks[b][t]) {
		game->baronies[b]->knights
		    -= game->attacks[b][t]->knights;
		game->baronies[b]->footmen
		    -= game->attacks[b][t]->footmen;
	    }

    /* process attacks on each barony in turn */
    for (b = 0; b < BARONIES; ++b)
	inbound_attacks (game, b);

    /* apply reported gains and losses */
    for (b = 0; b < BARONIES; ++b)
	for (t = 0; t < BARONIES; ++t)
	    if (game->reports[b] && game->reports[b]->attacks[t]) {
		apply_battle_results (game, b, t);
		free (game->attacks[b][t]);
		game->attacks[b][t] = NULL;
	    }
}

/**
 * Deliver the units ordered.
 * @param game is the game to process.
 */
static void deliver_units (game_t *game)
{
    /* local variables */
    int b; /* barony counter */

    /* loop through the baronies */
    for (b = 0; b < BARONIES; ++b)
	if (game->orders[b]) {

	    /* if order is paid for, deliver the units */
	    if (game->orders[b]->paid) {

		/* deliver the units */
		game->baronies[b]->castles += game->orders[b]->castles;
		game->baronies[b]->knights += game->orders[b]->knights;
		game->baronies[b]->footmen += game->orders[b]->footmen;

		/* ensure units do not break limits */
		if (game->baronies[b]->castles > 320)
		    game->baronies[b]->castles = 320;
		if (game->baronies[b]->knights > 3200)
		    game->baronies[b]->knights = 3200;
		if (game->baronies[b]->footmen > 32000)
		    game->baronies[b]->footmen = 32000;

		/* set the reports */
		if (! game->reports[b])
		    game->reports[b] = new_report (game->baronies[b]);
		game->reports[b]->delivered = new_unit_report
		    (game->orders[b]->castles, game->orders[b]->knights,
		     game->orders[b]->footmen);
	    }

	    /* if order is not paid, generate a report */
	    else if (game->orders[b]) {
		if (! game->reports[b])
		    game->reports[b] = new_report (game->baronies[b]);
		game->reports[b]->notdelivered = new_unit_report
		    (game->orders[b]->castles, game->orders[b]->knights,
		     game->orders[b]->footmen);
	    }

	    /* destroy the purchase order */
	    free (game->orders[b]);
	    game->orders[b] = NULL;
	}
}

/**
 * Process economic activity.
 * @param game is the game to process.
 */
static void process_economy (game_t *game)
{
    /* local variables */
    int b; /* barony counter */
    unsigned long int expenses; /* cost of upkeep of units */
    float desertion; /* fraction of military that deserts */

    /* loop through all the baronies */
    for (b = 0; b < BARONIES; ++b) {

	/* work out and apply population changes and taxes */
	if (! game->reports[b])
	    game->reports[b] = new_report (game->baronies[b]);
	game->reports[b]->migration
	    = (game->baronies[b]->land
	       - game->baronies[b]->population) / 2;
	game->reports[b]->tax =
	    game->baronies[b]->population / POPULATION_PER_GOLD;
	game->baronies[b]->population += game->reports[b]->migration;
	game->baronies[b]->gold += game->reports[b]->tax;

	/* work out expenses */
	expenses = calculate_expenses (game->baronies[b]);

	/* reduce expenses and military if not affordable */
	if (expenses > game->baronies[b]->gold) {
	    desertion = (1.0 - game->baronies[b]->gold / expenses) / 2;
	    game->reports[b]->attrition = new_unit_report
		(desertion * game->baronies[b]->castles,
		 desertion * game->baronies[b]->knights,
		 desertion * game->baronies[b]->footmen);
	    game->baronies[b]->castles *= (1 - desertion);
	    game->baronies[b]->knights *= (1 - desertion);
	    game->baronies[b]->footmen *= (1 - desertion);
	    expenses = game->baronies[b]->gold;
	}

	/* apply the expenses */
	game->baronies[b]->gold -= expenses;

	/* check that gold remains within limits */
	if (game->baronies[b]->gold > 32000)
	    game->baronies[b]->gold = 32000;
    }
}

/**
 * Calculate the rankings of the baronies.
 * @param game is the game to process.
 */
static void calculate_rankings (game_t *game)
{
    /* local variables */
    int order[BARONIES], /* order of the baronies */
	b, /* barony counter */
	sorted, /* flag indicates when sorting is done */
	o, /* temporary storage for barony order */
	ranking; /* ranking counter */

    /* initialise the order arbitrarily */
    for (b = 0; b < BARONIES; ++b)
	order[b] = b;
    
    /* sort the baronies by land */
    do {
	sorted = 1;
	for (b = 0; b < BARONIES - 1; ++b)
	    if (game->baronies[order[b]]->land
		< game->baronies[order[b + 1]]->land) {
		sorted = 0;
		o = order[b];
		order[b] = order[b + 1];
		order[b + 1] = o;
	    }
    } while (! sorted);

    /* allocate ranking numbers */
    ranking = game->baronies[order[0]]->ranking = 1;
    game->baronies[order[b]]->ranking = ranking;
    for (b = 1; b < BARONIES; ++b) {
	if (game->baronies[order[b]]->land
	    < game->baronies[order[b - 1]]->land)
	    ranking = b + 1;
	game->baronies[order[b]]->ranking = ranking;
    }
}

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Create and initialise a new game.
 * @return the new game.
 */
game_t *new_game (void)
{
    /* local variables */
    game_t *game; /* the game to create and return */
    int b, /* a counter for baronies */
	o, /* a counter for origins of attacks */
	t; /* a counter for targets of attacks */

    /* attempt to allocate memory */
    if (! (game = malloc (sizeof (game_t))))
	fatal_error (FATAL_MEMORY);

    /* initialise the turn */
    game->turn = 0;

    /* create the baronies */
    for (b = 0; b < BARONIES; ++b)
	game->baronies[b] = new_barony (names[b]);

    /* initialise the attacks */
    for (o = 0; o < BARONIES; ++o)
	for (t = 0; t < BARONIES; ++t)
	    game->attacks[o][t] = NULL;

    /* initialise the build and recruit orders */
    for (b = 0; b < BARONIES; ++b)
	game->orders[b] = NULL;

    /* initialise the reports */
    for (b = 0; b < BARONIES; ++b)
	game->reports[b] = NULL;

    /* return the new game */
    return game;
}

/**
 * Process a single turn.
 * @param game is the game to process.
 */
void process_turn (game_t *game)
{
    if (game->turn < 12) {
	clear_reports (game);
	take_payments (game);
	fight_battles (game);
	deliver_units (game);
	process_economy (game);
	calculate_rankings (game);
	++game->turn;
    }
}

/**
 * Clean up after a game.
 * @param game is the game to clean up.
 */
void end_game (game_t *game)
{
    /* local variables */
    int b, /* barony counter */
	o, /* a counter for origins of attacks */
	t; /* a counter for targets of attacks */

    /* free memory from the baronies */
    for (b = 0; b < BARONIES; ++b)
	if (game->baronies[b])
	    free (game->baronies[b]);

    /* free memory from the attacks */
    for (o = 0; o < BARONIES; ++o)
	for (t = 0; t < BARONIES; ++t)
	    if (game->attacks[o][t])
		free (game->attacks[o][t]);

    /* free memory from the reports */
    clear_reports (game);
    
    /* free memory from the game */
    free (game);
}


/**
 * Work out the maximum number of castles affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_castles_to_buy (game_t *game, int b)
{
    int population, /* population available */
	gold; /* gold available */

    /* initialise population and gold available */
    population = game->baronies[b]->population;
    gold = game->baronies[b]->gold;

    /* reduce resources according to what's allocated */
    if (game->orders[b]) {
	population = population
	    - game->orders[b]->knights * KNIGHT_COST
	    - game->orders[b]->footmen;
	gold = gold
	    - game->orders[b]->knights * KNIGHT_COST
	    - game->orders[b]->footmen;
    }

    /* return calculated value */
    return (gold < population ? gold : population) / CASTLE_COST;
}

/**
 * Work out the maximum number of knights affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_knights_to_buy (game_t *game, int b)
{
    int population; /* population available */
    int gold; /* gold available */

    /* initialise population and gold available */
    population = game->baronies[b]->population;
    gold = game->baronies[b]->gold;

    /* reduce resources according to what's allocated */
    if (game->orders[b]) {
	population = population
	    - game->orders[b]->castles * CASTLE_COST
	    - game->orders[b]->footmen;
	gold = gold
	    - game->orders[b]->castles * CASTLE_COST
	    - game->orders[b]->footmen;
    }

    /* return calculated value */
    return (gold < population ? gold : population) / KNIGHT_COST;
}

/**
 * Work out the maximum number of footmen affordable.
 * @param game is the game in progress.
 * @param b is the barony to check.
 */
int max_footmen_to_buy (game_t *game, int b)
{
    int population; /* population available */
    int gold; /* gold available */

    /* initialise population and gold available */
    population = game->baronies[b]->population;
    gold = game->baronies[b]->gold;

    /* reduce resources according to what's allocated */
    if (game->orders[b]) {
	population = population
	    - game->orders[b]->castles * CASTLE_COST
	    - game->orders[b]->knights * KNIGHT_COST;
	gold = gold
	    - game->orders[b]->castles * CASTLE_COST
	    - game->orders[b]->knights * KNIGHT_COST;
    }

    /* return calculated value */
    return (gold < population ? gold : population);
}

/**
 * Work out the number of knights available for attack.
 * @param game is the game in progress.
 * @param o is the barony originating the attack.
 * @param t is the target barony.
 */
int max_knights_to_send (game_t *game, int o, int t)
{
    int knights, /* knights available */
	b; /* barony counter */
    knights = game->baronies[o]->knights;
    for (b = 0; b < BARONIES; ++b)
	if (game->attacks[o][b] && b != t)
	    knights -= game->attacks[o][b]->knights;
    return knights;
}

/**
 * Work out the number of footmen available for attack.
 * @param game is the game in progress.
 * @param o is the barony originating the attack.
 * @param t is the target barony.
 */
int max_footmen_to_send (game_t *game, int o, int t)
{
    int footmen, /* footmen available */
	b; /* barony counter */
    footmen = game->baronies[o]->footmen;
    for (b = 0; b < BARONIES; ++b)
	if (game->attacks[o][b] && b != t)
	    footmen -= game->attacks[o][b]->footmen;
    return footmen;
}

/**
 * Work out the expenses for a barony's military.
 * @param barony is the barony concerned.
 */
unsigned long int calculate_expenses (barony_t *barony)
{
    return
	barony->castles * CASTLES_TAX / CASTLES_PER_TAX
	+ barony->knights * KNIGHTS_TAX / KNIGHTS_PER_TAX
	+ barony->footmen * FOOTMEN_TAX / FOOTMEN_PER_TAX;
}
