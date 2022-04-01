/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Building and Recruitment Order Handling Header.
 */

#ifndef __ORDER_H__
#define __ORDER_H__

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/* typdefs */
typedef struct order order_t;
typedef struct barony barony_t;

/** @struct order is the data for a building and recruitment order. */
struct order {

    /** @var barony is the barony making the instruction. */
    barony_t *barony;

    /** @var castles is the number of castles to build. */
    int castles;

    /** @var knights is the number of knights to train. */
    int knights;

    /** @var footmen is the number of footmen to draft. */
    int footmen;

    /** @var paid is 1 if the order is paid for, 0 if not. */
    int paid;

};

/*----------------------------------------------------------------------
 * Function Prototypes.
 */

/**
 * Create a new building and recruitment order.
 * @param barony is the barony placing the order.
 * @return the new order.
 */
order_t *new_order (barony_t *barony);

/**
 * Calculate the cost of an order in population.
 * @param order is the order to process.
 * @return the cost in population.
 */
int order_popcost (order_t *order);

/**
 * Calculate the cost of an order in gold.
 * @param order is the order to process.
 * @return the cost in gold.
 */
int order_goldcost (order_t *order);

#endif
