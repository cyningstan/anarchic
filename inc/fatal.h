/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Fatal Error Handler Header.
 */

#ifndef __FATAL_H__
#define __FATAL_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @enum fatal_t is the fatal error number enumerator. */
typedef enum {
    FATAL_OK, /* everything went OK */
    FATAL_MEMORY, /* out of memory error */
    FATAL_STRING, /* invalid string */
    FATAL_FILE, /* cannot open file */
    FATAL_DISPLAY, /* cannot open display */
    FATAL_CONFIG, /* config file missing or invalid */
    FATAL_EXPIRED, /* beta has expired */
    FATAL_LAST /* placeholder */
} fatal_t;

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Display a fatal error message and return to the OS.
 * @param errorcode is the fatal error code.
 */
void fatal_error (fatal_t errorcode);

#endif
