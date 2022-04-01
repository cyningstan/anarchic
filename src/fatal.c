/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Fatal Error Handler Header.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "fatal.h"
#include "display.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var messagess is an array of error messages */
static char *messages[FATAL_LAST] = {
    "testing termination",
    "out of memory",
    "invalid string",
    "cannot open file",
    "cannot open display",
    "config file anarchic.dat missing or invalid",
    "this beta version is expired"
};

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Display a fatal error message and return to the OS.
 * @param errorcode is the fatal error code.
 */
void fatal_error (fatal_t errorcode)
{
    display_close ();
    if (errorcode < FATAL_LAST)
	printf ("Error: %s.\n", messages[errorcode]);
    else
	printf ("Unknown error.n");
    exit (errorcode);
}
