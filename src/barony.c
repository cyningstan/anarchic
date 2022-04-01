/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Barony Module.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "barony.h"
#include "anarchic.h"
#include "fatal.h"

/*----------------------------------------------------------------------
 * Public Level Routines.
 */

/**
 * Create an intialise a new barony.
 * @param name is a name for the barony.
 * @return a pointer to the new barony.
 */
barony_t *new_barony (char *name) {

    /* local variables */
    barony_t *barony; /* the locally created barony to return */

    /* validation and memory reservation */
    if (! name || strlen (name) > 10)
	fatal_error (FATAL_STRING);
    if (! (barony = malloc (sizeof (barony_t))))
	fatal_error (FATAL_MEMORY);

    /* initialise the values */
    strcpy (barony->name, name);
    barony->control = CONTROL_COMPUTER;
    barony->land = 4000;
    barony->population = 4000;
    barony->gold = barony->population / POPULATION_PER_GOLD;
    barony->castles = 40;
    barony->knights = 400;
    barony->footmen = 4000;
    barony->ranking = 1;

    /* return the new barony */
    return barony;
    
}
