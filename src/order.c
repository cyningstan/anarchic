/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Building and Recruitment Order Handling Module.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* project headers */
#include "order.h"
#include "anarchic.h"
#include "fatal.h"

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Create a new building and recruitment order.
 * @param barony is the barony placing the order.
 * @return the new order.
 */
order_t *new_order (barony_t *barony)
{
    /* local variables */
    order_t *order; /* the new order */

    /* attempt to reserve memory */
    if (! (order = malloc (sizeof (order_t))))
	fatal_error (FATAL_MEMORY);

    /* initialise the order */
    order->barony = barony;
    order->castles = 0;
    order->knights = 0;
    order->footmen = 0;
    order->paid = 0;

    /* return the new order */
    return order;
}

/**
 * Calculate the cost of an order in population.
 * @param order is the order to process.
 * @return the cost in population.
 */
int order_popcost (order_t *order)
{
    return order->castles * CASTLE_COST
	+ order->knights * KNIGHT_COST
	+ order->footmen;
}

/**
 * Calculate the cost of an order in gold.
 * @param order is the order to process.
 * @return the cost in gold.
 */
int order_goldcost (order_t *order)
{
    return order->castles * CASTLE_COST
	+ order->knights * KNIGHT_COST
	+ order->footmen;
}
