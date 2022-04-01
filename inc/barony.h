/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Barony Header.
 */

#ifndef __BARONY_H__
#define __BARONY_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @enum control_t enumerates who can control a barony. */
typedef enum {
    CONTROL_HUMAN,
    CONTROL_COMPUTER,
    CONTROL_REMOTE
} control_t;

/** @struct barony is the barony structure. */
typedef struct barony barony_t;
struct barony {

    /** @var name is the name of the barony. */
    char name[11];

    /** @var control is who controls the barony */
    control_t control;

    /** @var land is the land owned by the barony. */
    int land;

    /** @var population is the population of the barony. */
    int population;

    /** @var gold is the barony's gold riches. */
    int gold;

    /** @var castles is the number of castles in the barony. */
    int castles;

    /** @var knights is the barony's force of knights. */
    int knights;

    /** @var footmen is the barony's army of footmen. */
    int footmen;

    /** @var ranking is the barony's ranking */
    int ranking;

};

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Create an intialise a new barony.
 * @param name is a name for the barony.
 * @return a pointer to the new barony.
 */
barony_t *new_barony (char *name);

#endif
