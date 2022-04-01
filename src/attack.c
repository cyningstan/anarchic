/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Attack Module.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "attack.h"
#include "fatal.h"
#include "barony.h"

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Create a new attack.
 * @return a pointer to the attack.
 */
attack_t *new_attack (barony_t *origin, barony_t *target, int knights,
		     int footmen)
{
    /* local variables */
    attack_t *attack; /* the attack to create */

    /* attempt to allocate memory */
    if (! (attack = malloc (sizeof (attack_t))))
	fatal_error (FATAL_MEMORY);

    /* put in the attack details */
    attack->origin = origin;
    attack->target = target;
    attack->knights = knights;
    attack->footmen = footmen;

    /* return the attack */
    return attack;
}
