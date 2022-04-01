/*======================================================================
 * Anarchic Kingdom
 * A light strategy game set in medieval times.
 * Copyright (C) Damian Gareth Walker 2021.
 *
 * Asset Generation Module.
 */

/*----------------------------------------------------------------------
 * Included headers.
 */

/* standard C headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* library headers */
#include "cgalib.h"

/* project headers */
#include "fatal.h"

/*----------------------------------------------------------------------
 * File Level Variables.
 */

/** @var output is the output file handle */
static FILE *output;

/** @var scr is the screen */
static Screen *scr = NULL;

/*----------------------------------------------------------------------
 * Level 1 Functions.
 */

/**
 * Open the output data file.
 */
static void open_dat_file (void)
{
    if (! (output = fopen("anarchic/anarchic.dat", "wb")))
	fatal_error (FATAL_FILE);
    fwrite ("ANK100G", 8, 1, output);
}

/**
 * Initialise the temporary output screen
 */
static void initialise_screen (void)
{
    if (! (scr = scr_create (4)))
	fatal_error (FATAL_DISPLAY);
    scr_palette (scr, 0, 15);
}

/**
 * Load the assets from a particular .PIC file.
 * @param asset_file_id is the file number, 1 or 2.
 */
static void load_assets (int asset_file_id)
{
    /* local variables */
    FILE *input; /* input file handle */
    char header[7], /* header from input file */
	*pic, /* pointer to picture buffer */
	filename[80]; /* name of input file */

    /* attempt to open the input file and read the header */
    sprintf (filename, "assets/anarch-%d.pic", asset_file_id);
    if (! (input = fopen (filename, "rb")))
	fatal_error (FATAL_FILE);
    if (! fread (header, 7, 1, input))
	fatal_error (FATAL_FILE);

    /* attempt to reserve memory for the picture */
    if (! (pic = malloc (16192)))
	fatal_error (FATAL_FILE);
    if (! (fread (pic, 16192, 1, input)))
	fatal_error (FATAL_FILE);

    /* put the picture on the screen and free the memory */
    _fmemcpy ((void far *) 0xb8000000, pic, 16192);
    free (pic);
}

/**
 * Grab the Cyningstan logo from the first asset file.
 */
static void copy_cyningstan_logo (void)
{
    Bitmap *bit; /* temporary reusable bitmap variable */
    bit = bit_create (128, 16);
    scr_get (scr, bit, 16, 64);
    bit_write (bit, output);
    bit_destroy (bit);
}

/**
 * Grab the game title from the first asset file.
 */
static void copy_title (void)
{
    Bitmap *bit; /* temporary reusable bitmap variable */
    bit = bit_create (192, 56);
    scr_get (scr, bit, 16, 8);
    bit_write (bit, output);
    bit_destroy (bit);
}

/**
 * Copy the cursor and its mask from the first asset file.
 */
static void copy_cursor (void)
{
    Bitmap *bit; /* temporary reusable bitmap variable */
    bit = bit_create (32, 32);
    scr_get (scr, bit, 208, 8);
    bit_write (bit, output);
    scr_get (scr, bit, 240, 8);
    bit_write (bit, output);
    bit_destroy (bit);
}

/**
 * Copy the title banner for all screens but the title.
 */
static void copy_banner (void)
{
    Bitmap *bit; /* temporary reusable bitmap */
    bit = bit_create (64, 16);
    scr_get (scr, bit, 144, 64);
    bit_write (bit, output);
    bit_destroy (bit);
}

/**
 * Copy the screen border from the first asset file.
 */
static void copy_border (void)
{
    Bitmap *bit; /* temporary reusable bitmap variable */
    scr_ink (scr, 0);
    scr_box (scr, 8, 8, 304, 176);
    bit = bit_create (320, 200);
    scr_get (scr, bit, 0, 0);
    bit_write (bit, output);
    bit_destroy (bit);
}

/**
 * Copy the barony icons from the second asset file.
 */
static void copy_icons (void)
{
    Bitmap *bit; /* temporary bitmap */
    int stat, /* stat type counter */
	size; /* size counter */
    bit = bit_create (32, 32);
    for (stat = 0; stat < 6; ++stat)
	for (size = 0; size < 3; ++size) {
	    scr_get (scr, bit, 32 * size, 32 * stat);
	    bit_write (bit, output);
	}
    bit_destroy (bit);
}

/**
 * Copy the barony terrain from the second asset file.
 */
static void copy_terrain (void)
{
    Bitmap *bit; /* temporary bitmap */
    int terrain, /* terrain type counter */
	side; /* side counter */
    bit = bit_create (112, 24);
    for (terrain = 0; terrain < 3; ++terrain)
	for (side = 0; side < 2; ++side) {
	    scr_get (scr, bit, 96 + 112 * side, 24 * terrain);
	    bit_write (bit, output);
	}
    bit_destroy (bit);
}

/**
 * Copy the capital city images from the second asset file.
 */
static void copy_capitals (void)
{
    Bitmap *bit; /* temporary bitmap */
    int row, /* row of capital */
	column; /* column of capital */
    bit = bit_create (80, 32);
    for (column = 0; column < 2; ++column)
	for (row = 0; row < 4; ++row) {
	    scr_get (scr, bit, 96 + 80 * column, 72 + 32 * row);
	    bit_write (bit, output);
	}
    bit_destroy (bit);
}

/**
 * Copy the barony shields from the second asset file.
 */
static void copy_shields (void)
{
    Bitmap *bit; /* temporary bitmap */
    int row, /* row of shield */
	column; /* column of shield */
    bit = bit_create (32, 32);
    for (column = 0; column < 2; ++column)
	for (row = 0; row < 4; ++row) {
	    scr_get (scr, bit, 256 + 32 * column, 72 + 32 * row);
	    bit_write (bit, output);
	}
    bit_destroy (bit);
}

/**
 * Copy the font from a font file.
 */
static void copy_font (void)
{
    /* local variables */
    FILE *input; /* input file handle */
    char header[8]; /* header read from input file */
    Font *fnt; /* font read from input file */

    /* open input file, read and verify header */
    if (! (input = fopen ("cgalib/cgalib/fnt/past.fnt", "rb")))
	fatal_error (FATAL_FILE);
    if (! (fread (header, 8, 1, input)))
	fatal_error (FATAL_FILE);
    if (strcmp (header, "CGA100F"))
	fatal_error (FATAL_FILE);

    /* read the font and copy it to the output file */
    if (! (fnt = fnt_read (input)))
	fatal_error (FATAL_FILE);
    fclose (input);
    fnt_write (fnt, output);
    fnt_destroy (fnt);
}

/**
 * Close the temporary output screen and return to text mode.
 */
static void close_screen (void)
{
    scr_destroy (scr);
}

/**
 * Close the output file.
 */
static void close_dat_file (void)
{
    fclose (output);
}

/*----------------------------------------------------------------------
 * Public Functions.
 */

/**
 * Main Function.
 * @return 0 for success, >0 for failure.
 */
int main (void)
{
    open_dat_file ();
    initialise_screen ();
    load_assets (1);
    copy_cyningstan_logo ();
    copy_title ();
    copy_cursor ();
    copy_banner ();
    copy_border ();
    load_assets (2);
    copy_icons ();
    copy_terrain ();
    copy_capitals ();
    copy_shields ();
    copy_font ();
    close_screen ();
    close_dat_file ();
    printf ("ANARCHIC\\ANARCHIC.DAT generated successfully.\n");
    return 0;
}

/* called by fatal error */
void display_close (void) {
    if (scr)
	scr_destroy (scr);
}

