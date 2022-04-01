/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Attack Header.
 */

#ifndef __ATTACK_H__
#define __ATTACK_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* typedefs */
typedef struct attack attack_t;
typedef struct barony barony_t;

/** @struct attack is the data for a single attack. */
struct attack {

    /** @var origin is the origin of the attack. */
    barony_t *origin;

    /** @var target is the target of the attack. */
    barony_t *target;

    /** @var knights is the number of knights sent. */
    int knights;

    /** @var footmen is the number of footmen sent. */
    int footmen;

};

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Create a new attack.
 * @return a pointer to the attack.
 */
attack_t *new_attack (barony_t *origin, barony_t *target, int knights,
		     int footmen);

#endif
