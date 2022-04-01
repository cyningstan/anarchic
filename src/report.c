/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Report Handling Module.
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
#include "report.h"
#include "fatal.h"
#include "barony.h"
#include "attack.h"

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Create and initialise a new report.
 * @param barony is the barony the report is addressed to.
 * @return the new report.
 */
report_t *new_report (barony_t *barony)
{
    /* local variables */
    report_t *report; /* the report to return */
    int b; /* barony counter */

    /* reservation of memory */
    if (! (report = malloc (sizeof (report_t))))
	fatal_error (FATAL_MEMORY);

    /* initialise barony */
    report->barony = barony;

    /* initialise the attack and defence reports */
    for (b = 0; b < BARONIES; ++b) {
	report->attacks[b] = NULL;
	report->defences[b] = NULL;
    }

    /* initialise economic activity */
    report->migration = 0;
    report->tax = 0;

    /* initialise unit reports */
    report->delivered = NULL;
    report->notdelivered = NULL;
    report->attrition = NULL;

    /* return the new report */
    return report;
}

/**
 * Create a unit report and stuff it with values.
 * @param castles is the number of castles built.
 * @param knights is the number of knights trained.
 * @param footmen is the number of footmen drafted.
 */
unit_report_t *new_unit_report (int castles, int knights, int footmen)
{
    /* local variables */
    unit_report_t *report; /* report to return */

    /* allocate memory */
    if (! (report = malloc (sizeof (unit_report_t))))
	fatal_error (FATAL_MEMORY);

    /* allocate the values */
    report->castles = castles;
    report->knights = knights;
    report->footmen = footmen;

    /* return the report */
    return report;
}

/**
 * Destroy a report when no longer needed.
 * @param report is the report to destroy.
 */
void destroy_report (report_t *report)
{
    /* local variables */
    int b; /* barony counter */

    /* free memory from battle reports */
    for (b = 0; b < BARONIES; ++b) {
	if (report->attacks[b])
	    free (report->attacks[b]);
	if (report->defences[b])
	    free (report->defences[b]);
    }

    /* free memory from unit reports */
    if (report->delivered)
	free (report->delivered);
    if (report->notdelivered)
	free (report->notdelivered);
    if (report->attrition)
	free (report->attrition);

    /* free memory from reports in general */
    free (report);
}
