/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Display Handler Module for Graphical Output.
 */

/*----------------------------------------------------------------------
 * Included Headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* compiler specific headers */
#include <conio.h>
#include <dos.h>

/* project headers */
#include "cgalib.h"
#include "display.h"
#include "fatal.h"
#include "game.h"
#include "barony.h"
#include "report.h"
#include "order.h"
#include "attack.h"

/*----------------------------------------------------------------------
 * Data Definitions.
 */

/** @var scr is the CGALIB screen. */
static Screen *scr = NULL;

/** @var title is the title screen bitmap. */
static Bitmap *title;

/** @var cursor is the cursor bitmap. */
static Bitmap *cursor;

/** @var cursor_mask is the cursor mask. */
static Bitmap *cursor_mask;

/** @var banner is the title banner. */
static Bitmap *banner;

/** @var border is the universal tapestry border. */
static Bitmap *border;

/** @var icons are the landscape icons. */
static Bitmap *icons[6][3];

/** @var terrain is the landscape horizon terrain. */
static Bitmap *terrain[3][2];

/** @var capitals are the capital city bitmaps. */
static Bitmap *capitals[8];

/** @var shields are the barony shield bitmaps. */
static Bitmap *shields[8];

/** @var fonts are the various coloured fonts. */
static Font *fonts[4];

/** @var buffer is a screen buffer for building up displays. */
static Bitmap *buffer;

/** @var control_names are the names of who can control baronies. */
static char *control_names[] = {
    "Human",
    "Computer",
    "Remote"
};

/** @var months are the names of the months. */
static char *months[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

/** @var own_barony_menu is the menu for your barony. */
static char *own_barony_menu[] = {
    "Report",
    "Spend",
    "Previous",
    "Next",
    "Done"
};

/** @var other_barony_menu is the menu for other baronies. */
static char *other_barony_menu[] = {
    "Attack",
    "Previous",
    "Next",
    "Done"
};

/** @var end_barony_menu is the menu for an end barony. */
static char *end_barony_menu[] = {
    "Report",
    "Done"
};

/** @var rankings are ordinal numbers for the rankings. */
static char *rankings[] = {
    "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th"
};

/** @var humans is the number of human players. */
static int humans;

/** @var player is the player currently in control. */
static int player;

/** @var viewed is the barony currently viewed. */
static int viewed;

/** @var horizons represents each barony's horizon terrain. */
static int horizons[9];

/** @var positions orders the baronies' icons in their landscape. */
static int positions[8][36];

/*----------------------------------------------------------------------
 * Service Level Functions.
 */

/**
 * Centre a piece of text within a given width.
 * @param text is the text to centre.
 * @param width is the width of the field.
 * @return a pointer to a persistent static text buffer.
 */
static char *centre (char *text, int width)
{
    static char centred[81]; /* persistent text buffer */

    sprintf (centred, "%.*s%s%.*s",
	     (width - strlen (text)) / 2,
	     "                                        ",
	     text,
	     width - strlen (text) - (width - strlen (text)) / 2,
	     "                                        ");
    return centred;
}

/**
 * Display a number on the screen buffer.
 * It is assumed that a suitable font has already been selected.
 * @param x is the x coordinate.
 * @param y is the y coordinate.
 * @param number is the number to display.
 */
static void show_number (int x, int y, int number)
{
    char output[6]; /* representation of the output */
    sprintf (output, "%5d", number);
    bit_print (buffer, x, y, output);
}

/**
 * Redraw a section of the buffer onto the screen.
 * @param x is the top left x coordinate.
 * @param y is the top left y coordinate.
 * @param w is the width of the section.
 * @param h is the height of the section.
 */
static void redraw (int x, int y, int w, int h)
{
    Bitmap *section; /* section to redraw */
    if (! (section = bit_create (w, h)))
	fatal_error (FATAL_MEMORY);
    bit_get (buffer, section, x, y);
    scr_put (scr, section, x, y, DRAW_PSET);
    bit_destroy (section);
}

/*----------------------------------------------------------------------
 * Level 2 Functions.
 */

/**
 * Show icons on the landscape.
 * @param quantity is the quantity to process.
 * @param stat is the icon group (stat) to display.
 * @param start is the starting position in the positions list.
 */
static void show_icons (int quantity, int stat)
{
    char icon_list[7]; /* list of up to 6 icons */
    int c, /* entry counter in the icon list */
	x, /* x position of the icon */
	y, /* y position of the icon */
	i, /* icon size */
	position; /* position in the barony's positions order */

    /* knights and castles have multipliers */
    if (stat == 3)
	quantity *= 100;
    else if (stat == 4)
	quantity *= 10;

    /* work out the string to return */
    if (quantity < 100)
	strcpy (icon_list, "");
    else if (quantity < 500)
	strcpy (icon_list, "0");
    else if (quantity < 1000)
	strcpy (icon_list, "00");
    else if (quantity < 4000)
	strcpy (icon_list, "100");
    else if (quantity < 7000)
	strcpy (icon_list, "1100");
    else if (quantity < 10000)
	strcpy (icon_list, "1110");
    else if (quantity < 20000)
	strcpy (icon_list, "21110");
    else if (quantity < 30000)
	strcpy (icon_list, "22110");
    else
	strcpy (icon_list, "222110");

    /* display the icons in the icon list */
    for (c = 0; c < strlen (icon_list); ++c) {
	position = positions[viewed][6 * stat + c];
	x = position % 9;
	y = position / 9;
	x = 8 + 32 * x + 16 * (y & 1);
	y = 56 + 32 * y;
	i = icon_list[c] - '0';
	bit_put (buffer, icons[stat][i], x, y, DRAW_PSET);
    }
}

/**
 * Confirm that the user wishes to exit, should Esc be pressed.
 * @return 1 if yes, 0 if no.
 */
static int confirm_exit (void)
{
    int ch; /* character input */
    scr_ink (scr, 3);
    scr_font (scr, fonts[2]);
    scr_print (scr, 0, 192, centre ("Press Esc again to exit.", 80));
    ch = getch ();
    if (! ch)
	ch = -getch ();
    redraw (0, 192, 320, 8);
    return (ch == 27);
}

/*----------------------------------------------------------------------
 * Level 1 Functions.
 */

/**
 * Load the assets from the asset file.
 */
static void load_assets (void)
{
    FILE *input; /* input file */
    char header[8]; /* the input file header */
    Bitmap *logo; /* the Cyningstan logo */
    int row, /* row counter */
	column; /* column counter */

    /* open the asset file */
    if (! (input = fopen ("anarchic.dat", "rb")))
	fatal_error (FATAL_CONFIG);
    if (! (fread (&header, 8, 1, input)))
	fatal_error (FATAL_CONFIG);
    if (strcmp (header, "ANK100G"))
	fatal_error (FATAL_CONFIG);

    /* display the Cyningstan logo */
    if (! (logo = bit_read (input)))
	fatal_error (FATAL_CONFIG);
    scr_put (scr, logo, 96, 92, DRAW_PSET);
    bit_destroy (logo);

    /* load all the title screen assets */
    if (! (title =  bit_read (input)))
	fatal_error (FATAL_CONFIG);
    if (! (cursor = bit_read (input)))
	fatal_error (FATAL_CONFIG);
    if (! (cursor_mask = bit_read (input)))
	fatal_error (FATAL_CONFIG);
    if (! (banner = bit_read (input)))
	fatal_error (FATAL_CONFIG);
    if (! (border = bit_read (input)))
	fatal_error (FATAL_CONFIG);

    /* load the other in-game assets */
    for (row = 0; row < 6; ++row)
	for (column = 0; column < 3; ++column)
	    if (! (icons[row][column] = bit_read (input)))
		fatal_error (FATAL_CONFIG);
    for (row = 0; row < 3; ++row)
	for (column = 0; column < 2; ++column)
	    if (! (terrain[row][column] = bit_read (input)))
		fatal_error (FATAL_CONFIG);
    for (row = 0; row < 8; ++row)
	if (! (capitals[row] = bit_read (input)))
	    fatal_error (FATAL_CONFIG);
    for (row = 0; row < 8; ++row)
	if (! (shields[row] = bit_read (input)))
	    fatal_error (FATAL_CONFIG);

    /* load the font and initialise the screen with it */
    if (! (fonts[3] = fnt_read (input)))
	fatal_error (FATAL_CONFIG);
    for (row = 0; row < 3; ++row)
	fonts[row] = fnt_copy (fonts[3]);
    fnt_colours (fonts[0], 0, 2);
    fnt_colours (fonts[1], 1, 0);
    fnt_colours (fonts[2], 2, 0);
    scr_font (scr, fonts[3]);

    /* close the file */
    fclose (input);
}

/**
 * Initialise the horizons of the baronies.
 */
static void init_horizons (void)
{
    int b, /* barony counter */
	o, /* other barony to swap with */
	s; /* swapped value */

    /* initialise horizon combinations in order */
    for (b = 0; b < 9; ++b) {
	horizons[b] = b;
    }

    /* scramble the order of the horizons */
    for (b = 0; b < 9; ++b) {
	o = rand () % 9;
	s = horizons[b];
	horizons[b] = horizons[o];
	horizons[o] = s;
    }
}

/**
 * Initialise the landscape ordering of the baronies.
 */
static void init_landscapes (void)
{
    int b, /* barony counter */
	p, /* position counter */
	o, /* other position to swap with */
	s; /* swapped value */

    /* main loop */
    for (b = 0; b < 8; ++b) {

	/* initialise the icons in order */
	for (p = 0; p < 36; ++p)
	    positions[b][p] = p;

	/* randomise the icon positions */
	for (p = 0; p < 36; ++p) {
	    o = rand () % 36;
	    s = positions[b][o];
	    positions[b][o] = positions[b][p];
	    positions[b][p] = s;
	}
    }
}

/**
 * Clear the screen area leaving the tapestry border.
 */
static void clear_screen (void)
{
    bit_ink (buffer, 0);
    bit_box (buffer, 8, 8, 304, 176);
    bit_box (buffer, 0, 192, 320, 8);
    bit_ink (buffer, 3);
}

/**
 * Display a title banner.
 * @param heading is the heading text to put on the banner.
 */
static void show_heading (char *heading)
{
    bit_put (buffer, banner, 128, 8, DRAW_PSET);
    bit_font (buffer, fonts[1]);
    bit_print (buffer, 136, 10, centre (heading, 12));
}

/**
 * Display a prompt at the bottom of the screen.
 * @param prompt is the prompt to display.
 */
static void show_prompt (char *prompt)
{
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 0, 192, centre (prompt, 80));
}

/**
 * Display the currently viewed barony on a prepared tapestry.
 */
static void show_barony (int turn, barony_t *barony)
{
    /* display the barony name */
    show_heading (barony->name);

    /* display textual information */
    bit_font (buffer, fonts[1]);
    if (turn < 12)
	bit_print (buffer, 8, 8, months[turn]);
    else
	bit_print (buffer, 8, 8, "End game");
    bit_print (buffer, 300, 8,
	       rankings[barony->ranking - 1]);
    show_number (64, 8, barony->land);
    show_number (64, 16, barony->population);
    show_number (64, 24, barony->gold);
    show_number (204, 8, barony->castles);
    show_number (204, 16, barony->knights);
    show_number (204, 24, barony->footmen);

    /* display the labels */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 88, 8, "Land");
    bit_print (buffer, 88, 16, "Peasants");
    bit_print (buffer, 88, 24, "Gold");
    bit_print (buffer, 228, 8, "Castles");
    bit_print (buffer, 228, 16, "Knights");
    bit_print (buffer, 228, 24, "Footmen");
    bit_print (buffer, 276, 8, "Rank:");

    /* display the horizon and capital */
    bit_put (buffer, terrain[horizons[viewed] / 3][0], 8, 32,
	     DRAW_PSET);
    bit_put (buffer, terrain[horizons[viewed] % 3][1], 200, 32,
	     DRAW_PSET);
    bit_put (buffer, capitals[barony->ranking - 1], 120, 24,
	     DRAW_PSET);

    /* display the landscape icons */
    show_icons (barony->land, 0);
    show_icons (barony->population, 1);
    show_icons (barony->gold, 2);
    show_icons (barony->castles, 3);
    show_icons (barony->knights, 4);
    show_icons (barony->footmen, 5);
}

/**
 * Show a menu at the bottom of the screen, and get an option.
 * @param options is the number of options.
 * @param menu_text is an array of strings.
 * @param shortcuts is a string of other shortcut keys.
 */
char show_menu (int options, char **menu_text, char *shortcuts)
{
    int option, /* option */
	left, /* x coordinate of leftmost menu option */
	done, /* set to 1 when an option or shortcut has been chosen */
	o, /* option counter */
	ch; /* character input */

    /* initialise the menu */
    left = 320 - 40 * options;
    done = 0;
    option = 0;

    /* display the initial menu */
    bit_ink (buffer, 0);
    bit_box (buffer, 0, 192, left, 8);
    bit_ink (buffer, 3);
    bit_font (buffer, fonts[2]);
    for (o = 0; o < options; ++o)
	bit_print (buffer, left + 40 * o, 192,
		   centre (menu_text[o], 10));

    /* main loop */
    redraw (0, 192, 320, 8);
    do {

	/* get the keypress */
	bit_font (buffer, fonts[0]);
	bit_print (buffer, left + 40 * option, 192,
		   centre (menu_text[option], 10));
	redraw (left + 40 * option, 192, 40, 8);
	ch = getch ();
	if (! ch)
	    ch = -getch ();
	bit_font (buffer, fonts[2]);
	bit_print (buffer, left + 40 * option, 192,
		   centre (menu_text[option], 10));

	/* process the cursor keys */
	if (ch == -75 && option > 0) {
	    scr_put (scr, buffer, 0, 0, DRAW_PSET);
	    --option;
	} else if (ch == -77 && option < options - 1) {
	    scr_put (scr, buffer, 0, 0, DRAW_PSET);
	    ++option;
	}

	/* process space */
	if (ch == ' ' || ch == 13) {
	    scr_put (scr, buffer, 0, 0, DRAW_PSET);
	    ch = *menu_text[option];
	    done = 1;
	}

	/* process initial menu characters */
	for (o = 0; o < options; ++o)
	    if (toupper (ch) == menu_text[o][0]) {
		ch = *menu_text[o];
		done = 1;
	    }

	/* process shortcut keys */
	if (strchr (shortcuts, ch))
	    done = 1;
	
    } while (! done);
    
    /* return the chosen option */
    return ch;
}

/**
 * Alter a numeric value by steps.
 * @param value is the initial value.
 * @param direction is the direction of alteration.
 * @param max is the maximum allowed value.
 * @return the modified value.
 */
static int alter_by_steps (int value, int direction, int max)
{
    long int result; /* the result to return */
    int step; /* step by which to alter the value */

    /* calculate the step */
    if (value == 0 || value % 10)
	step = 1;
    else if (value % 100)
	step = 10;
    else if (value % 1000)
	step = 100;
    else if (value % 10000)
	step = 1000;
    else
	step = 10000;
    if (value + direction * step == 0 && step != 1)
	step /= 10;

    /* apply the step */
    result = value + direction * step;
    if (result < 0)
	result = 0;
    else if (result > max)
	result = max;

    /* return the result */
    return result;
}

/**
 * Alter a numeric value by appending a digit.
 * @param value is the initial value.
 * @param digit is the digit to append.
 * @param max is the maximum allowed value.
 * @return the modified value.
 */
static int alter_by_digit (int value, int digit, int max)
{
    unsigned long int result; /* the result to return */
    result = 10 * value + digit;
    if (result > max)
	return value;
    else
	return result;
}

/**
 * Show a prompt and await SPACE or ENTER.
 */
static int report_key (void)
{
    int ch; /* character input */
    show_prompt ("Press SPACE for next report.");
    scr_put (scr, buffer, 0, 0, DRAW_PSET);
    do {
	ch = getch ();
	if (! ch) ch = -getch ();
	if (ch == 27 && confirm_exit ())
	    return 1;
    } while (ch != ' ' && ch != 13);
    return 0;
}

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Initialise the display handler.
 */
state_t display_open (int argc, char **argv)
{
    time_t start; /* time the Cyningstan logo was displayed */
    int c, /* argument counter */
	mono, /* mono graphics selected */
	ugly; /* ugly palette selected */

    /* check command line arguments */
    mono = ugly = 0;
    for (c = 1; c < argc; ++c)
	if (! strcmp (argv[c], "-p") || ! strcmp (argv[c], "-P"))
	    ugly = 1;
	else if (! strcmp (argv[c], "-m") || ! strcmp (argv[c], "-M"))
	    mono = 1;

    /* initialise the graphics screen */
    if (! (scr = scr_create (mono ? 6 : 4)))
	fatal_error (FATAL_DISPLAY);
    if (! ugly)
	scr_palette (scr, 0, 15);
    start = time (NULL);

    /* load the assets, displaying the logo */
    load_assets ();

    /* initialise the layout of the barony screens */
    srand (time (0));
    init_horizons ();
    init_landscapes ();

    /* initialise the screen buffer */
    if (! (buffer = bit_create (320, 200)))
	fatal_error (FATAL_MEMORY);

    /* clear the logo after at least three seconds and return */
    while (time (NULL) < start + 4);
    scr_ink (scr, 0);
    scr_box (scr, 96, 92, 128, 16);
    sleep (1);
    return DISPLAY_NEW_GAME;
}

/**
 * Handle the New Game screen.
 * @param game is the game to configure.
 * @return the new display state.
 */
state_t display_new_game (game_t *game)
{
    int r, /* row counter */
	c, /* column counter */
	b, /* barony counter */
	x, /* x coordinate for display */
	y, /* y coordinate for display */
	ch; /* character input */

    /* count the humans */
    humans = 0;
    for (b = 0; b < 8; ++b)
	if (game->baronies[b]->control == CONTROL_HUMAN)
	    ++humans;
    
    /* build up the initial display */
    bit_put (buffer, border, 0, 0, DRAW_PSET);
    bit_put (buffer, title, 64, 8, DRAW_PSET);
    bit_put (buffer, icons[0][2], 16, 16, DRAW_PSET);
    bit_put (buffer, icons[4][2], 16, 80, DRAW_PSET);
    bit_put (buffer, icons[3][2], 16, 144, DRAW_PSET);
    bit_put (buffer, icons[1][2], 272, 16, DRAW_PSET);
    bit_put (buffer, icons[5][2], 272, 80, DRAW_PSET);
    bit_put (buffer, icons[5][0], 272, 144, DRAW_PSET);
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 272, 176, "S");
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 276, 176, "tart game");
    
    /* display the barony information */
    for (r = 0; r < 2; ++r)
	for (c = 0; c < 4; ++c) {
	    b = c + 4 * r;
	    bit_put (buffer, shields[b], 72 + 48 * c, 80 + 64 * r,
		     DRAW_PSET);
	    bit_font (buffer, fonts[1]);
	    bit_print (buffer, 68 + 48 * c, 72 + 64 * r,
		       centre (game->baronies[b]->name, 10));
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 72 + 48 * c, 112 + 64 * r,
		       centre (control_names
				[game->baronies[b]->control], 8));
	}

    /* display the prompt */
    show_prompt
	("Select the baronies for human control, and start the game");

    /* copy the buffer over the screen */
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* initialise the control loop */
    scr_put (scr, buffer, 0, 0, DRAW_PSET);
    b = 0;
    do {
	
	/* display the cursor and await a key */
	if (b < 4) {
	    x = 72 + 48 * b;
	    y = 80;
	} else if (b < 8) {
	    x = 72 + 48 * (b - 4);
	    y = 144;
	} else {
	    x = 272;
	    y = 144;
	}
	bit_put (buffer, cursor_mask, x, y, DRAW_AND);
	bit_put (buffer, cursor, x, y, DRAW_OR);
	redraw (x, y, 32, 32);
	ch = getch ();
	if (! ch) ch = -getch ();
	bit_put (buffer, cursor_mask, x, y, DRAW_AND);

	/* react to cursor keys */
	if (ch == -72 && b >= 4 && b <= 7) {
	    redraw (x, y, 32, 32);
	    b -= 4;
	} else if (ch == -80 && b < 4) {
	    redraw (x, y, 32, 32);
	    b += 4;
	} else if (ch == -75 && b != 0 && b != 4) {
	    redraw (x, y, 32, 32);
	    --b;
	} else if (ch == -77 && b != 3 && b != 8) {
	    redraw (x, y, 32, 32);
	    ++b;
	}

	/* react to shortcut keys */
	else if (ch >= '1' && ch <= '8') {
	    if (b != ch -'1') {
		redraw (x, y, 32, 32);
		b = ch - '1';
	    }
	    ch = ' ';
	} else if (ch == 's' || ch == 'S') {
	    redraw (x, y, 32, 32);
	    b = 8;
	    ch = ' ';
	}

	/* react to space key */
	if (ch == 13) ch = ' ';
	if (ch == ' ' && b != 8) {
	    humans += game->baronies[b]->control;
	    game->baronies[b]->control = ! game->baronies[b]->control;
	    humans -= game->baronies[b]->control;
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 72 + 48 * (b & 3), 112 + 64 * (b / 4),
		       centre (control_names
			       [game->baronies[b]->control], 8));
	    if (humans == 1)
		player = viewed = b;
	    redraw (72 + 48 * (b & 3), 112 + 64 * (b / 4), 32, 8);
	}

	/* react to attempt to start game without any humans */
	else if (ch == ' ' && b == 8 && humans == 0) {
	    show_prompt
		("Select at least 1 human player to start the game!");
	    redraw (0, 192, 320, 8);
	}

	/* react to Esc */
	else if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;

    } while (ch != ' ' || b != 8 || humans == 0);

    /* return */
    return DISPLAY_INTRODUCTION;
}

/**
 * Hand the Introduction screen.
 * @param game is the game to start.
 * @return the new display state.
 */
state_t display_introduction (game_t *game)
{
    int ch; /* character input */
    
    /* initialise the screen */
    clear_screen ();
    show_heading ("Anarchy!");

    /* print the story */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 80, 24,
	       "The old king has died, leaving his young");
    bit_print (buffer, 80, 32, "son not yet ready to rule the");
    bit_print (buffer, 80, 40,
	       "kingdom. The minority of the young king");
    bit_print (buffer, 80, 48, "will last another year, during which");
    bit_print (buffer, 80, 56, "anarchy reigns!");
    bit_print (buffer, 80, 72,
	       "Eight of the most powerful lords of the");
    bit_print (buffer, 80, 80, "kingdom will take advantage of this");
    bit_print (buffer, 80, 88,
	       "period of anarchy, bolstering their own");
    bit_print (buffer, 80, 96, "power in order to have the most");
    bit_print (buffer, 80, 104,
	       "influence over the young king when he");
    bit_print (buffer, 80, 112, "takes control.");
    bit_print (buffer, 80, 128, "Build stout castles, train faithful");
    bit_print (buffer, 80, 136,
	       "knights, draft a multitude of footmen,");
    bit_print (buffer, 80, 144,
	       "and set about your opponents to take as");
    bit_print (buffer, 80, 152,
	       "much of their land and gold as you can");
    bit_print (buffer, 80, 160, "over the next twelve months.");
    bit_print (buffer, 80, 176, "Good luck!");

    /* display some decoration in the margins */
    bit_put (buffer, icons[4][2], 24, 80, DRAW_PSET);
    bit_put (buffer, icons[3][2], 264, 80, DRAW_PSET);

    /* display the prompt and wait for the key */
    show_prompt ("Press SPACE to continue.");
    scr_put (scr, buffer, 0, 0, DRAW_PSET);
    do {
	ch = getch ();
	if (! ch) ch = -getch ();
	if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;
    } while (ch != 32 && ch != 13);

    /* on to the next screen */
    return humans == 1 ?
	DISPLAY_OWN_BARONY :
	DISPLAY_MAIN_MENU;
}

/**
 * Handle the Main Menu screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_main_menu (game_t *game)
{
    int state, /* the current state */
	r, /* row counter */
	c, /* column counter */
	b, /* barony counter */
	x, /* x coordinate for display */
	y, /* y coordinate for display */
	ch; /* character input */

    /* initialise the screen */
    state = DISPLAY_MAIN_MENU;
    clear_screen ();
    show_heading (months[game->turn]);

    /* display the barony information */
    for (r = 0; r < 2; ++r)
	for (c = 0; c < 4; ++c) {
	    b = c + 4 * r;
	    bit_put (buffer, shields[b], 72 + 48 * c, 48 + 64 * r,
		     DRAW_PSET);
	    if (game->baronies[b]->control == CONTROL_HUMAN) {
		bit_font (buffer, fonts[1]);
		bit_print (buffer, 68 + 48 * c, 40 + 64 * r,
			   centre (game->baronies[b]->name, 10));
	    }
	}

    /* display the "end turn" icon */
    bit_put (buffer, icons[5][0], 272, 112, DRAW_PSET);
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 272, 144, "E");
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 276, 144, "nd turn");

    /* display the prompt, and update everything we've displayed */
    show_prompt ("Select the next barony to play, or end the turn.");
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* main input loop */
    b = 0;
    do {

	/* display the cursor and await a key */
	if (b < 4) {
	    x = 72 + 48 * b;
	    y = 48;
	} else if (b < 8) {
	    x = 72 + 48 * (b - 4);
	    y = 112;
	} else {
	    x = 272;
	    y = 112;
	}
	bit_put (buffer, cursor_mask, x, y, DRAW_AND);
	bit_put (buffer, cursor, x, y, DRAW_OR);
	redraw (x, y, 32, 32);
	ch = getch ();
	if (! ch) ch = -getch ();
	bit_put (buffer, cursor_mask, x, y, DRAW_AND);

	/* react to cursor keys */
	if (ch == -72 && b >= 4 && b <= 7) {
	    redraw (x, y, 32, 32);
	    b -= 4;
	} else if (ch == -80 && b < 4) {
	    redraw (x, y, 32, 32);
	    b += 4;
	} else if (ch == -75 && b != 0 && b != 4) {
	    redraw (x, y, 32, 32);
	    --b;
	} else if (ch == -77 && b != 3 && b != 8) {
	    redraw (x, y, 32, 32);
	    ++b;
	}

	/* react to shortcut keys */
	else if (ch >= '1' && ch <= '8') {
	    redraw (x, y, 32, 32);
	    b = ch - '1';
	    ch = ' ';
	} else if (ch == 'e' || ch == 'E') {
	    redraw (x, y, 32, 32);
	    b = 8;
	    ch = ' ';
	}

	/* react to space key */
	if (ch == 13) ch = ' ';
	if (ch == ' ' && b != 8 &&
	    game->baronies[b]->control == CONTROL_HUMAN) {
	    viewed = player = b;
	    state = DISPLAY_OWN_BARONY;
	} else if (ch == ' ' && b == 8) {
	    state = DISPLAY_END_TURN;
	}

	/* react to Esc key */
	if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;

    } while (state == DISPLAY_MAIN_MENU);

    /* return the chosen state */
    return state;
}

/**
 * Handle the Own Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_own_barony (game_t *game)
{
    int ch; /* option returned from menu */
    
    /* initialise the screen */
    clear_screen ();
    show_barony (game->turn, game->baronies[viewed]);
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* main menu loop */
    do {
	ch = show_menu (5, own_barony_menu, "12345678\x1b");
	switch (ch) {
	case '\x1b':
	    if (confirm_exit ()) return DISPLAY_QUIT;
	    break;
	case 'D':
	    return humans == 1 ? DISPLAY_END_TURN : DISPLAY_MAIN_MENU;
	case 'S':
	    return DISPLAY_ORDER;
	case 'R':
	    return DISPLAY_REPORT;
	case 'N':
	case 'P':
	    viewed = (viewed + (ch == 'N') - (ch == 'P')) & 7;
	    return DISPLAY_OTHER_BARONY;
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8':
	    viewed = ch - '1';
	    return viewed == player ?
		DISPLAY_OWN_BARONY :
		DISPLAY_OTHER_BARONY;
	}
    } while (1);
}

/**
 * Handle the Barony Selector screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_select_barony (game_t *game)
{
    return DISPLAY_QUIT;
}

/**
 * Handle the Other Barony screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_other_barony (game_t *game)
{
    int ch; /* option returned from menu */
    
    /* initialise the screen */
    clear_screen ();
    show_barony (game->turn, game->baronies[viewed]);
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* main menu loop */
    do {
	ch = show_menu (4, other_barony_menu, "12345678\x1b");
	switch (ch) {
	case '\x1b':
	    if (confirm_exit ()) return DISPLAY_QUIT;
	    break;
	case 'D':
	    return humans == 1 ? DISPLAY_END_TURN : DISPLAY_MAIN_MENU;
	case 'A':
	    return DISPLAY_ATTACK;
	case 'N':
	case 'P':
	    viewed = (viewed + (ch == 'N') - (ch == 'P')) & 7;
	    return viewed == player ?
		DISPLAY_OWN_BARONY :
		DISPLAY_OTHER_BARONY;
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8':
	    viewed = ch - '1';
	    return viewed == player ?
		DISPLAY_OWN_BARONY :
		DISPLAY_OTHER_BARONY;
	}
    } while (1);
}

/**
 * Handle the Order screen.
 * @param game is the game to play.
 * @return the new display state.
 */
state_t display_order (game_t *game)
{
    int option, /* option selected */
	ch; /* character input */

    /* initialise values */
    if (! game->orders[player])
	game->orders[player] = new_order (game->baronies[player]);
    option = 0;
    
    /* initialise the screen */
    clear_screen ();
    show_heading ("Forces");

    /* show forces icons */
    bit_put (buffer, icons[3][1], 56, 40, DRAW_PSET);
    bit_put (buffer, icons[4][1], 56, 72, DRAW_PSET);
    bit_put (buffer, icons[5][1], 56, 104, DRAW_PSET);

    /* show labels */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 96, 48, "Build");
    bit_print (buffer, 100, 56, "astles");
    bit_print (buffer, 96, 80, "Train");
    bit_print (buffer, 100, 88, "nights");
    bit_print (buffer, 96, 112, "Draft");
    bit_print (buffer, 100, 120, "ootmen");
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 96, 56, "C");
    bit_print (buffer, 96, 88, "K");
    bit_print (buffer, 96, 120, "F");

    /* show pricing */
    bit_font (buffer, fonts[1]);
    bit_print (buffer, 136, 48, "100");
    bit_print (buffer, 136, 56, "100");
    bit_print (buffer, 140, 80, "10");
    bit_print (buffer, 140, 88, "10");
    bit_print (buffer, 144, 112, "1");
    bit_print (buffer, 144, 120, "1");
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 152, 48, "Peasants");
    bit_print (buffer, 152, 56, "Gold");
    bit_print (buffer, 152, 80, "Peasants");
    bit_print (buffer, 152, 88, "Gold");
    bit_print (buffer, 152, 112, "Peasant");
    bit_print (buffer, 152, 120, "Gold");

    /* show maximum available */
    bit_print (buffer, 192, 48, "Max:");
    bit_print (buffer, 192, 80, "Max:");
    bit_print (buffer, 192, 112, "Max:");
    bit_font (buffer, fonts[1]);
    show_number (204, 56, max_castles_to_buy (game, player));
    show_number (204, 88, max_knights_to_buy (game, player));
    show_number (204, 120, max_footmen_to_buy (game, player));

    /* show numbers currently ordered */
    show_number (236, 56, game->orders[player]->castles);
    show_number (236, 88, game->orders[player]->knights);
    show_number (236, 120, game->orders[player]->footmen);

    /* show "Done" option */
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 240, 152, "D");
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 244, 152, "one");

    /* add the prompt and update the whole display */
    show_prompt
	("Enter the numbers of castles, knights and footmen to add");
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* main input loop */
    bit_font (buffer, fonts[1]);
    do {
 
	/* show the cursor, get a key */
	bit_put (buffer, cursor, 232, 40 + 32 * option, DRAW_OR);
	redraw (232, 40 + 32 * option, 32, 32);
	ch = getch ();
	if (! ch)
	    ch = -getch ();
	bit_put (buffer, cursor_mask, 232, 40 + 32 * option, DRAW_AND);

	/* process increase/decrease keys */
	if (ch == -75 || ch == -77) {
	    switch (option) {
	    case 0:
		game->orders[player]->castles = alter_by_steps
		    (game->orders[player]->castles, -76 - ch,
		     max_castles_to_buy (game, player));
		show_number (236, 56, game->orders[player]->castles);
		break;
	    case 1:
		game->orders[player]->knights = alter_by_steps
		    (game->orders[player]->knights, -76 - ch,
		     max_knights_to_buy (game, player));
		show_number (236, 88, game->orders[player]->knights);
		break;
	    case 2:
		game->orders[player]->footmen = alter_by_steps
		    (game->orders[player]->footmen, -76 - ch,
		     max_footmen_to_buy (game, player));
		show_number (236, 120, game->orders[player]->footmen);
		break;
	    }
	    redraw (236, 56 + 32 * option, 20, 8);

	    /* update max limits */
	    show_number (204, 56, max_castles_to_buy (game, player));
	    show_number (204, 88, max_knights_to_buy (game, player));
	    show_number (204, 120, max_footmen_to_buy (game, player));
	    redraw (204, 56, 20, 8);
	    redraw (204, 88, 20, 8);
	    redraw (204, 120, 20, 8);
	}

	/* process digit keys */
	if (ch >= '0' && ch <= '9') {
	    switch (option) {
	    case 0:
		game->orders[player]->castles = alter_by_digit
		    (game->orders[player]->castles, ch - '0',
		     max_castles_to_buy (game, player));
		show_number (236, 56, game->orders[player]->castles);
		break;
	    case 1:
		game->orders[player]->knights = alter_by_digit
		    (game->orders[player]->knights, ch - '0',
		     max_knights_to_buy (game, player));
		show_number (236, 88, game->orders[player]->knights);
		break;
	    case 2:
		game->orders[player]->footmen = alter_by_digit
		    (game->orders[player]->footmen, ch - '0',
		     max_footmen_to_buy (game, player));
		show_number (236, 120, game->orders[player]->footmen);
		break;
	    }
	    redraw (236, 56 + 32 * option, 20, 8);

	    /* update maximum limits */
	    show_number (204, 56, max_castles_to_buy (game, player));
	    show_number (204, 88, max_knights_to_buy (game, player));
	    show_number (204, 120, max_footmen_to_buy (game, player));
	    redraw (204, 56, 20, 8);
	    redraw (204, 88, 20, 8);
	    redraw (204, 120, 20, 8);
	}

	/* process backspace */
	if (ch == 8) {
	    switch (option) {
	    case 0:
		game->orders[player]->castles /= 10;
		show_number (236, 56, game->orders[player]->castles);
		break;
	    case 1:
		game->orders[player]->knights /= 10;
		show_number (236, 88, game->orders[player]->knights);
		break;
	    case 2:
		game->orders[player]->footmen /= 10;
		show_number (236, 120, game->orders[player]->footmen);
		break;
	    }
	    redraw (236, 56 + 32 * option, 20, 8);

	    /* update maximum limits */
	    show_number (204, 56, max_castles_to_buy (game, player));
	    show_number (204, 88, max_knights_to_buy (game, player));
	    show_number (204, 120, max_footmen_to_buy (game, player));
	    redraw (204, 56, 20, 8);
	    redraw (204, 88, 20, 8);
	    redraw (204, 120, 20, 8);
	}

	/* process movement keys */
	if (ch == -72 && option > 0) {
	    redraw (232, 40 + 32 * option, 32, 32);
	    --option;
	} else if (ch == -80 && option < 3) {
	    redraw (232, 40 + 32 * option, 32, 32);
	    ++option;
	}

	/* process shortcut keys */
	if (ch == 'C' || ch == 'c') {
	    redraw (232, 40 + 32 * option, 32, 32);
	    option = 0;
	} else if (ch == 'K' || ch == 'k') {
	    redraw (232, 40 + 32 * option, 32, 32);
	    option = 1;
	} else if (ch == 'F' || ch == 'f') {
	    redraw (232, 40 + 32 * option, 32, 32);
	    option = 2;
	}
	
	/* process enter, and space on done */
	if (ch == 13 && option < 3) {
	    redraw (232, 40 + 32 * option, 32, 32);
	    ++option;
	} else if ((ch == ' ' || ch == 13) && option == 3)
	    ch = 'D';

	/* process Esc */
	if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;

    } while (ch != 'D' && ch != 'd');

    /* remove the order if no units were ordered */
    if (game->orders[player]->castles +
	game->orders[player]->knights +
	game->orders[player]->footmen == 0) {
	free (game->orders[player]);
	game->orders[player] = NULL;
    }

    /* back to looking at one's own barony when finished */
    return DISPLAY_OWN_BARONY;
}

/**
 * Handle the Attack Screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_attack (game_t *game)
{
    int knights, /* knights to send */
	footmen, /* footmen to send */
	option, /* option selected */
	ch; /* character input */

    /* get current attack numbers */
    if (game->attacks[player][viewed]) {
	knights = game->attacks[player][viewed]->knights;
	footmen = game->attacks[player][viewed]->footmen;
	free (game->attacks[player][viewed]);
	game->attacks[player][viewed] = NULL;
    } else
	knights = footmen = 0;

    /* initialise the screen */
    clear_screen ();
    show_heading ("Attack");
    option = 0;

    /* display the two baronies */
    bit_font (buffer, fonts[1]);
    bit_print (buffer, 68, 32,
	       centre (game->baronies[player]->name, 10));
    bit_print (buffer, 212, 32, centre
	       (game->baronies[viewed]->name, 10));
    bit_put (buffer, shields[player], 72, 40, DRAW_PSET);
    bit_put (buffer, shields[viewed], 216, 40, DRAW_PSET);
    bit_put (buffer, icons[4][0], 72, 72, DRAW_PSET);
    bit_put (buffer, icons[5][0], 72, 104, DRAW_PSET);

    /* display the unit icons */
    bit_put (buffer, icons[4][1], 104, 40, DRAW_PSET);
    bit_put (buffer, icons[3][1], 184, 40, DRAW_PSET);

    /* display the prompts */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 120, 80, "Send");
    bit_print (buffer, 124, 88, "nights");
    bit_print (buffer, 120, 112, "Send");
    bit_print (buffer, 124, 120, "ootmen");
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 120, 88, "K");
    bit_print (buffer, 120, 120, "F");

    /* display the maximum numbers to send */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 168, 80, "Max:");
    bit_print (buffer, 168, 112, "Max:");
    bit_font (buffer, fonts[1]);
    show_number (180, 88, max_knights_to_send (game, player, viewed));
    show_number (180, 120, max_footmen_to_send (game, player, viewed));

    /* display the current attack strength */
    show_number (220, 88, knights);
    show_number (220, 120, footmen);

    /* display "Done" prompt */
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 228, 152, "one");
    bit_font (buffer, fonts[2]);
    bit_print (buffer, 224, 152, "D");
    show_prompt ("Enter the numbers of knights and footmen to send.");
    bit_font (buffer, fonts[1]);

    /* main input loop */
    scr_put (scr, buffer, 0, 0, DRAW_PSET);
    do {

	/* show the cursor, get a key */
	bit_put (buffer, cursor, 216, 72 + 32 * option, DRAW_OR);
	redraw (216, 72 + 32 * option, 32, 32);
	ch = getch ();
	if (! ch)
	    ch = -getch ();

	/* process increase/decrease keys */
	if (ch == -75 || ch == -77) {
	    switch (option) {
	    case 0:
		knights = alter_by_steps
		    (knights, -76 - ch,
		     max_knights_to_send (game, player, viewed));
		show_number (220, 88, knights);
		break;
	    case 1:
		footmen = alter_by_steps
		    (footmen, -76 - ch,
		     max_footmen_to_send (game, player, viewed));
		show_number (220, 120, footmen);
		break;
	    }
	    redraw (220, 88 + 32 * option, 32, 32);
	}

	/* process digit keys */
	if (ch >= '0' && ch <= '9') {
	    switch (option) {
	    case 0:
		knights = alter_by_digit
		    (knights, ch - '0',
		     max_knights_to_send (game, player, viewed));
		show_number (220, 88, knights);
		break;
	    case 1:
		footmen = alter_by_digit
		    (footmen, ch - '0',
		     max_footmen_to_send (game, player, viewed));
		show_number (220, 120, footmen);
		break;
	    }
	    redraw (220, 88 + 32 * option, 32, 32);
	}

	/* process backspace */
	if (ch == 8) {
	    switch (option) {
	    case 0:
		knights /= 10;
		show_number (220, 88, knights);
		break;
	    case 1:
		footmen /= 10;
		show_number (220, 120, footmen);
		break;
	    }
	    redraw (220, 88 + 32 * option, 32, 32);
	}

	/* process movement keys */
	if (ch == -72 && option > 0) {
	    bit_put (buffer, cursor_mask, 216, 72 + 32 * option,
		     DRAW_AND);
	    redraw (216, 72 + 32 * option, 32, 32);
	    --option;
	} else if (ch == -80 && option < 2) {
	    bit_put (buffer, cursor_mask, 216, 72 + 32 * option,
		     DRAW_AND);
	    redraw (216, 72 + 32 * option, 32, 32);
	    ++option;
	}

	/* process shortcut keys */
	else if (ch == 'K' || ch == 'k') {
	    bit_put (buffer, cursor_mask, 216, 72 + 32 * option,
		     DRAW_AND);
	    redraw (216, 72 + 32 * option, 32, 32);
	    option = 0;
	} else if (ch == 'F' || ch == 'f') {
	    bit_put (buffer, cursor_mask, 216, 72 + 32 * option,
		     DRAW_AND);
	    redraw (216, 72 + 32 * option, 32, 32);
	    option = 1;
	}
	
	/* process enter, and space on done */
	if (ch == 13 && option < 2) {
	    bit_put (buffer, cursor_mask, 216, 72 + 32 * option,
		     DRAW_AND);
	    redraw (216, 72 + 32 * option, 32, 32);
	    ++option;
	} else if ((ch == ' ' || ch == 13) && option == 2)
	    ch = 'D';

	/* process Esc */
	if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;

    } while (ch != 'D' && ch != 'd');
    
    /* launch attack if any forces are committed */
    if (knights + footmen > 0)
        game->attacks[player][viewed] =
	    new_attack (game->baronies[player],
			game->baronies[viewed],
			knights, footmen);

    /* return to the barony view */
    return DISPLAY_OTHER_BARONY;
}

/**
 * Display a Barony Report.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_report (game_t *game)
{
    report_t *report; /* report shorthand */
    int b, /* barony counter */
	ch; /* character input */
    unsigned long int expenses; /* calculated expenses */

    /* initialise the screen */
    clear_screen ();
    show_heading ("Report");

    /* return if there is no report */
    if (! (report = game->reports[player])) {
	bit_font (buffer, fonts[3]);
	bit_print (buffer, 108, 96, "There is nothing to report!");
	show_prompt ("Press SPACE to continue.");
	scr_put (scr, buffer, 0, 0, DRAW_PSET);
	do {
	    ch = getch ();
	    if (ch == 27 && confirm_exit ())
		return DISPLAY_QUIT;
	} while (ch != ' ' && ch != 13);
	return DISPLAY_OWN_BARONY;
    }

    /* report on attacks sent out */
    for (b = 0; b < BARONIES; ++b)
	if (report->attacks[b]) {

	    /* display the icon and clear the text area */
	    bit_put (buffer, icons[4][1], 40, 88, DRAW_PSET);
	    bit_ink (buffer, 0);
	    bit_box (buffer, 80, 80, 200, 48);
	    bit_ink (buffer, 3);

	    /* display the boilerplate text */
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 80, 80, "You attacked the barony of ");
	    bit_print (buffer, 80, 88,
		       "with ..... knights and ..... footmen.");
	    bit_print (buffer, 80, 96,
		       "You took ..... land and looted ..... gold.");
	    bit_print (buffer, 80, 104,
		       "You took ..... castles and razed .....,");
	    bit_print (buffer, 80, 112, "and slew ..... footmen.");
	    bit_print (buffer, 80, 120,
		       "You lost ..... knights and ..... footmen.");

	    /* fill in the fields */
	    bit_font (buffer, fonts[1]);
	    bit_print (buffer, 188, 80,
		       report->attacks[b]->target->name);
	    show_number (100, 88, report->attacks[b]->knights_sent);
	    show_number (172, 88, report->attacks[b]->footmen_sent);
	    show_number (116, 96, report->attacks[b]->land_taken);
	    show_number (204, 96, report->attacks[b]->gold_looted);
	    show_number (116, 104, report->attacks[b]->castles_taken);
	    show_number (212, 104, report->attacks[b]->castles_razed);
	    show_number (116, 112, report->attacks[b]->footmen_slain);
	    show_number (116, 120, report->attacks[b]->knights_lost);
	    show_number (188, 120, report->attacks[b]->footmen_lost);

	    /* show prompt and await keypress */
	    if (report_key ())
		return DISPLAY_QUIT;
	}

    /* report on defence against attacks sent against us */
    for (b = 0; b < BARONIES; ++b)
	if (report->defences[b]) {

	    /* display the icon and clear the text area */
	    bit_put (buffer, icons[3][1], 40, 88, DRAW_PSET);
	    bit_ink (buffer, 0);
	    bit_box (buffer, 80, 80, 200, 48);
	    bit_ink (buffer, 3);

	    /* display the boilerplate text */
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 80, 80,
		       "You were attacked by the barony of ");
	    bit_print (buffer, 80, 88,
		       "with ..... knights and ..... footmen.");
	    bit_print (buffer, 80, 96,
		       "They took ..... land and looted ..... gold.");
	    bit_print (buffer, 80, 104,
		       "They took ..... castles and razed .....,");
	    bit_print (buffer, 80, 112, "and slew ..... footmen.");
	    bit_print (buffer, 80, 120,
		       "They lost ..... knights and ..... footmen.");

	    /* fill in the fields */
	    bit_font (buffer, fonts[1]);
	    bit_print (buffer, 220, 80,
		       report->defences[b]->origin->name);
	    show_number (100, 88, report->defences[b]->knights_sent);
	    show_number (172, 88, report->defences[b]->footmen_sent);
	    show_number (120, 96, report->defences[b]->land_taken);
	    show_number (208, 96, report->defences[b]->gold_looted);
	    show_number (120, 104, report->defences[b]->castles_taken);
	    show_number (216, 104, report->defences[b]->castles_razed);
	    show_number (116, 112, report->defences[b]->footmen_slain);
	    show_number (120, 120, report->defences[b]->knights_lost);
	    show_number (192, 120, report->defences[b]->footmen_lost);

	    /* show prompt and await keypress */
	    if (report_key ())
		return DISPLAY_QUIT;
	}

    /* population migration and tax income */
    bit_ink (buffer, 0);
    bit_box (buffer, 40, 80, 240, 48);
    bit_ink (buffer, 3);
    bit_put (buffer, icons[1][2], 56, 88, DRAW_PSET);
    bit_font (buffer, fonts[3]);
    if (report->migration > 0)
	bit_print (buffer, 96, 96,
		   "..... people settled in your barony.");
    else if (report->migration < 0)
	bit_print (buffer, 96, 96,
		   "..... people fled from your barony.");
    bit_print (buffer, 96, 104, "Your people paid ..... gold in tax.");
    bit_font (buffer, fonts[1]);
    if (report->migration)
	show_number (96, 96, report->migration > 0 ?
		     report->migration :
		     -report->migration);
    show_number (164, 104, report->tax);
    if (report_key ())
	return DISPLAY_QUIT;

    /* new forces */
    if (report->delivered) {

	/* display the icon and clear the text area */
	bit_ink (buffer, 0);
	bit_box (buffer, 40, 80, 240, 48);
	bit_ink (buffer, 3);
	bit_put (buffer, icons[5][2], 72, 88, DRAW_PSET);

	/* show the boilerplate text */
	bit_font (buffer, fonts[3]);
	bit_print (buffer, 112, 88, "You built ..... castles,");
	bit_print (buffer, 112, 96, "trained ..... knights,");
	bit_print (buffer, 112, 104, "and drafted ..... footmen.");

	/* fill in the fields */
	bit_font (buffer, fonts[1]);
	show_number (152, 88, report->delivered->castles);
	show_number (144, 96, report->delivered->knights);
	show_number (160, 104, report->delivered->footmen);

	/* await keypress */
	if (report_key ())
	    return DISPLAY_QUIT;
    }

    /* unaffordable forces */
    if (report->notdelivered) {

	/* display the icon and clear the text area */
	bit_ink (buffer, 0);
	bit_box (buffer, 40, 80, 240, 48);
	bit_ink (buffer, 3);
	bit_put (buffer, icons[2][0], 72, 88, DRAW_PSET);

	/* show the boilerplate text */
	bit_font (buffer, fonts[3]);
	bit_print (buffer, 112, 88, "You could not build ..... castles,");
	bit_print (buffer, 112, 96, "nor train ..... knights,");
	bit_print (buffer, 112, 104, "nor draft ..... footmen.");
	bit_print (buffer, 112, 112, "due to lack of resources.");

	/* fill in the fields */
	bit_font (buffer, fonts[1]);
	show_number (192, 88, report->notdelivered->castles);
	show_number (152, 96, report->notdelivered->knights);
	show_number (152, 104, report->notdelivered->footmen);

	/* await keypress */
	if (report_key ())
	    return DISPLAY_QUIT;
    }

    /* desertion */
    if (report->attrition) {
	
	/* display the icon and clear the text area */
	bit_ink (buffer, 0);
	bit_box (buffer, 40, 80, 240, 48);
	bit_ink (buffer, 3);
	bit_put (buffer, icons[5][0], 72, 88, DRAW_PSET);

	/* show the boilerplate text */
	bit_font (buffer, fonts[3]);
	bit_print (buffer, 112, 88, "Due to lack of funds,");
	bit_print (buffer, 112, 96, "..... castles fell into ruin,");
	bit_print (buffer, 112, 104,
		   "..... knights deserted your cause,");
	bit_print (buffer, 112, 112,
		   "and ..... footmen died of starvation.");

	/* fill in the fields */
	bit_font (buffer, fonts[1]);
	show_number (112, 96, report->attrition->castles);
	show_number (112, 104, report->attrition->knights);
	show_number (128, 112, report->attrition->footmen);

	/* await keypress */
	if (report_key ())
	    return DISPLAY_QUIT;
    }

    /* outgoings */
    expenses = calculate_expenses (report->barony);
    bit_ink (buffer, 0);
    bit_box (buffer, 40, 80, 240, 48);
    bit_ink (buffer, 3);
    bit_put (buffer, icons[2][2], 56, 88, DRAW_PSET);
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 96, 88, "You paid ..... gold for the upkeep of");
    bit_print (buffer, 96, 96, "..... castles,");
    bit_print (buffer, 96, 104, "..... knights, and");
    bit_print (buffer, 96, 112, "..... footmen.");
    bit_font (buffer, fonts[1]);
    show_number (132, 88, expenses);
    show_number (96, 96, report->barony->castles);
    show_number (96, 104, report->barony->knights);
    show_number (96, 112, report->barony->footmen);
    if (report_key ())
	return DISPLAY_QUIT;

    /* return to the barony view */
    return game->turn < 12 ? DISPLAY_OWN_BARONY : DISPLAY_END_BARONY;
}

/**
 * Handle the End Turn screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_turn (game_t *game)
{
    char message[77]; /* message to display */
    int b; /* barony counter */

    /* initialise the screen */
    clear_screen ();
    show_heading ("End turn");

    /* show the message for 3 seconds */
    sprintf (message, "%s turn is being processed...",
	     months[game->turn]);
    bit_font (buffer, fonts[3]);
    bit_print (buffer, 8, 96, centre (message, 76));
    show_prompt ("Please wait...");
    scr_put (scr, buffer, 0, 0, DRAW_PSET);
    sleep (3);

    /* if one human player, go straight to their barony afterwards */
    if (humans == 1)
	for (b = 0; b < BARONIES; ++b)
	    if (game->baronies[b]->control == CONTROL_HUMAN) {
		player = viewed = b;
		return game->turn == 11 ?
		    DISPLAY_END_GAME :
		    DISPLAY_OWN_BARONY;
	    }

    /* otherwise go to the main menu afterwards */
    return game->turn == 11 ?
	DISPLAY_END_GAME :
	DISPLAY_MAIN_MENU;
}

/**
 * Handle the End Game screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_game (game_t *game)
{
    int b, /* barony counter */
	r, /* ranking counter */
	o, /* order counter */
	ch, /* character input */
	order[BARONIES]; /* order of baronies */

    /* sort the baronies into rank order */
    for (o = 0, r = 1; r <= BARONIES; ++r)
	for (b = 0; b < BARONIES; ++b)
	    if (game->baronies[b]->ranking == r)
		order[o++] = b;

    /* initialise the screen */
    clear_screen ();
    show_heading ("Game Over!");
    show_prompt ("Select baronies to view, or press ESC to end.");

    /* display the barony rankings */
    for (o = 0; o < BARONIES; ++o) {
	b = order[o];

	/* even-numbered rankings on the right */
	if (o & 1) {
	    bit_put (buffer, shields[b], 160, 32 + 16 * o, DRAW_PSET);
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 200, 40 + 16 * o,
		       rankings[game->baronies[b]->ranking - 1]);
	    bit_font (buffer, fonts[1]);
	    bit_print (buffer, 200, 48 + 16 * o,
		       game->baronies[b]->name);
	}

	/* odd-numbered rankings on the left */
	else {
	    bit_put (buffer, shields[b], 128, 32 + 16 * o, DRAW_PSET);
	    bit_font (buffer, fonts[3]);
	    bit_print (buffer, 108, 40 + 16 * o,
		       rankings[game->baronies[b]->ranking - 1]);
	    bit_font (buffer, fonts[1]);
	    bit_print (buffer,
		       120 - 4 * strlen (game->baronies[b]->name),
		       48 + 16 * o,
		       game->baronies[b]->name);
	}
    }

    /* set cursor to last barony viewed */
    for (o = 0; o < BARONIES - 1; ++o)
	if (order[o] == viewed)
	    break;

    /* main input loop */
    do {

	/* show the cursor and await a menu option */
	bit_put (buffer, cursor, 128 + 32 * (o & 1), 32 + 16 * o,
		 DRAW_OR);
	scr_put (scr, buffer, 0, 0, DRAW_PSET);
	ch = getch ();
	if (! ch)
	    ch = -getch ();
	bit_put (buffer, cursor_mask, 128 + 32 * (o & 1), 32 + 16 * o,
		 DRAW_AND);

	/* process digit keys to view a barony */
	if (ch >= '1' && ch <= '8') {
	    for (o = 0; o < BARONIES; ++o) {
		if (order[o] == ch - '1')
		    break;
	    }
	    ch = ' ';
	}

	/* move up and down with the cursor keys */
	if (ch == -72 && o > 0)
	    --o;
	else if (ch == -80 && o < BARONIES - 1)
	    ++o;

	/* process Esc */
	if (ch == 27 && confirm_exit ())
	    return DISPLAY_QUIT;

    } while (ch != ' ' && ch != 13);

    /* barony selected */
    viewed = order[o];
    return DISPLAY_END_BARONY;
}

/**
 * Handle the End Barony screen.
 * @param game is the game in play.
 * @return the new display state.
 */
state_t display_end_barony (game_t *game)
{
    int ch; /* option returned from menu */
    
    /* initialise the screen */
    clear_screen ();
    show_barony (game->turn, game->baronies[viewed]);
    scr_put (scr, buffer, 0, 0, DRAW_PSET);

    /* wait for a key and return */
    do {

	/* if looking at a human barony, offer a report */
	if (game->baronies[viewed]->control == CONTROL_HUMAN) {
	    ch = show_menu (2, end_barony_menu, "\x1b");
	    if (ch == 27 && confirm_exit ())
		return DISPLAY_QUIT;
	    if (ch == 'R')
		return DISPLAY_REPORT;
	    if (ch == 'D')
		return DISPLAY_END_GAME;
	}

	/* if looking at a computer barony, just return to rankings */
	else {
	    show_prompt ("Press SPACE to continue.");
	    redraw (0, 192, 320, 8);
	    ch = getch ();
	    if (ch == 27 && confirm_exit ())
		return DISPLAY_QUIT;
	    if (ch == ' ' || ch == 13)
		return DISPLAY_END_GAME;
	}
    } while (1);
}

/**
 * Clean up the display handler.
 */
void display_close (void)
{
    int row, /* row counter */
	column; /* column counter */

    /* return early if we didn't get as far as preparing the screen */
    if (!scr)
	return;

    /* destroy bitmaps */
    bit_destroy (title);
    bit_destroy (cursor);
    bit_destroy (cursor_mask);
    bit_destroy (border);
    for (row = 0; row < 6; ++row)
	for (column = 0; column < 3; ++column)
	    bit_destroy (icons[row][column]);
    for (row = 0; row < 3; ++row)
	for (column = 0; column < 2; ++column)
	    bit_destroy (terrain[row][column]);
    for (row = 0; row < 8; ++row) {
	bit_destroy (capitals[row]);
	bit_destroy (shields[row]);
    }

    /* destroy fonts */
    for (row = 0; row < 4; ++row)
	fnt_destroy (fonts[row]);

    /* return to text mode */
    scr_destroy (scr);
}
