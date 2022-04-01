/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Report Handling Header.
 */

#ifndef __REPORT_H__
#define __REPORT_H__

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* main header required for some constant definitions */
#include "anarchic.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* defined types for structures */
typedef struct report report_t;
typedef struct barony barony_t;
typedef struct attack_report attack_report_t;
typedef struct unit_report unit_report_t;

/** @struct report is the data for a report. */
struct report {

    /** @var barony is the barony the report is addressed to. */
    barony_t *barony;

    /** @var outbound are outbound attack reports. */
    attack_report_t *attacks[BARONIES];

    /** @var inbound are inbound attack reports. */
    attack_report_t *defences[BARONIES];

    /** @var migration is the population change. */
    int migration;

    /** @var tax is the tax revenue. */
    int tax;

    /** @var delivered are units successfully added. */
    unit_report_t *delivered;

    /** @var notdelivered are units that could not be afforded. */
    unit_report_t *notdelivered;

    /** @var attrition are units lost to attrition. */
    unit_report_t *attrition;

};

/** @struct attack_report is a report on an attack. */
struct attack_report {

    /** @var origin is the barony whence the attack came. */
    barony_t *origin;

    /** @var target is the target of the attack. */
    barony_t *target;

    /** @var knights_sent is the number of knights sent. */
    int knights_sent;

    /** @var footmen_sent is the number of footmen sent. */
    int footmen_sent;

    /** @var landtaken is the amount of land taken in the attack. */
    int land_taken;

    /** @var goldlooted is the amount of gold looted. */
    int gold_looted;

    /** @var castlestaken is the number of castles taken. */
    int castles_taken;

    /** @var castlesrazed is the number of castles razed. */
    int castles_razed;

    /** @var knightslost is the number of attacking knights lost. */
    int knights_lost;

    /** @var footmenlost is the number of attacking footmen lost. */
    int footmen_lost;

    /** @var footmenslain is the number of defending footmen slain. */
    int footmen_slain;

};

/** @struct unit_report is a report on units ordered. */
struct unit_report {

    /** @var castles are castles to be built. */
    int castles;

    /** @var knights are knights to be trained. */
    int knights;

    /** @var footmen are footmen to be drafted. */
    int footmen;
    
};

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Create and initialise a new report.
 * @param barony is the barony the report is addressed to.
 * @return the new report.
 */
report_t *new_report (barony_t *barony);

/**
 * Create a unit report and stuff it with values.
 * @param castles is the number of castles built.
 * @param knights is the number of knights trained.
 * @param footmen is the number of footmen drafted.
 */
unit_report_t *new_unit_report (int castles, int knights, int footmen);

/**
 * Destroy a report when no longer needed.
 * @param report is the report to destroy.
 */
void destroy_report (report_t *report);

#endif
