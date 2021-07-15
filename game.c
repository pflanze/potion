#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __C64__
# include <c64.h>
# include <conio.h>
#endif

#include "game.h"
#include "room.h"
#include "input.h"
#include "perform.h"
#include "output.h"

/* Global variables */
#ifndef __C64__
struct gamedata_s *gamedata = NULL;
char *input = NULL;
const char *object_string = NULL;
int screenwidth;
#endif

void initgame(void);
void intro(void);

int main(void)
{
	NR unsigned char exit;
	NR unsigned char oldroom;
	NR const char *s;

	initgame();
	intro();

	oldroom = 0;
	do
	{
		NR unsigned char verb;

		if (oldroom != gamedata->room)
		{
			printroom();
			oldroom = gamedata->room;
		}

		verb = getinput();
		exit = perform(verb);
	} while (GAME_CONTINUES == exit);

	switch (exit)
	{
		case GAME_LOST:
			s = "GAME OVER!";
			break;

		case GAME_WON:
			s = "Congratulations!";
			break;
	}

	PUTS(s);

#ifndef __C64__
	free(gamedata);
	free(input);
#endif

	return 0;
}

static const signed char defaultrooms[OBJECTS] =
{
	1, 2, 3, 7, 8, 9, 13, 14, 15, -1, 9,
	18, 16, -1, 24, 28, 19, 25, 26, 20, -1
};

static const unsigned char defaultmap[MAPSIZE] =
{
	             EXIT_SOUTH |             EXIT_EAST /*  1 */,
	                          EXIT_WEST | EXIT_EAST /*  2 */,
	             EXIT_SOUTH | EXIT_WEST             /*  3 */,
	             EXIT_SOUTH | EXIT_WEST             /*  4 */,
	             EXIT_SOUTH |             EXIT_EAST /*  5 */,
	                          EXIT_WEST             /*  6 */,
	EXIT_NORTH | EXIT_SOUTH                         /*  7 */,
	             EXIT_SOUTH                         /*  8 */,
	EXIT_NORTH | EXIT_SOUTH                         /*  9 */,
	EXIT_NORTH | EXIT_SOUTH                         /* 10 */,
	EXIT_NORTH | EXIT_SOUTH | EXIT_WEST | EXIT_EAST /* 11 */,
	             EXIT_SOUTH | EXIT_WEST             /* 12 */,
	EXIT_NORTH |                          EXIT_EAST /* 13 */,
	EXIT_NORTH |              EXIT_WEST             /* 14 */,
	EXIT_NORTH                                      /* 15 */,
	EXIT_NORTH |                          EXIT_EAST /* 16 */,
	EXIT_NORTH | EXIT_SOUTH | EXIT_WEST | EXIT_EAST /* 17 */,
	EXIT_NORTH | EXIT_SOUTH | EXIT_WEST             /* 18 */,
	             EXIT_SOUTH |             EXIT_EAST /* 19 */,
	             EXIT_SOUTH |             EXIT_EAST /* 20 */,
	             EXIT_SOUTH | EXIT_WEST | EXIT_EAST /* 21 */,
	                                      EXIT_EAST /* 22 */,
	EXIT_NORTH | EXIT_SOUTH | EXIT_WEST | EXIT_EAST /* 23 */,
	EXIT_NORTH | EXIT_SOUTH | EXIT_WEST             /* 24 */,
	EXIT_NORTH                                      /* 25 */,
	EXIT_NORTH |                          EXIT_EAST /* 26 */,
	EXIT_NORTH |              EXIT_WEST             /* 27 */,
	                                      EXIT_EAST /* 28 */,
	EXIT_NORTH |              EXIT_WEST | EXIT_EAST /* 29 */,
	                          EXIT_WEST             /* 30 */
};

void initgame(void)
{
	NR unsigned char i;

#ifndef __C64__
	gamedata = malloc(sizeof (struct gamedata_s));
	input = malloc(1024);
#endif

#ifdef SMALL
# if INVENTORY == 2
	gamedata->inventory[0] = -1;
	gamedata->inventory[1] = -1;
# else
#  error "No in-lining"
# endif
#else
	for (i = 0; i < INVENTORY; ++ i)
		gamedata->inventory[i] = -1;
#endif

	gamedata->room = 15;
	gamedata->pilleaten = 0;
	gamedata->seenscientist = 0;
	gamedata->roadvisit = 0;
#ifdef SMALL
	__asm__("ldx #%b", (unsigned char) OBJECTS - 1);
	__asm__("resetobjects: lda %v,x", defaultrooms);
	__asm__("sta %w,x", (unsigned int) gamedata->objects);
	__asm__("dex");
	__asm__("bpl resetobjects");

	__asm__("ldx #%b", (unsigned char) MAPSIZE - 1);
	__asm__("resetmap: lda %v,x", defaultmap);
	__asm__("sta %w,x", (unsigned int) gamedata->map);
	__asm__("dex");
	__asm__("bpl resetmap");
#else
	memcpy(gamedata->objects, defaultrooms, OBJECTS);
	memcpy(gamedata->map, defaultmap, MAPSIZE);
#endif
}

void intro(void)
{
#ifdef __C64__
	/* Set black screen */
	VIC.bgcolor0 = 0;
	VIC.bordercolor = 0;

	/* Turn off any sprites */
	VIC.spr_ena = 0;

	/* Clear screen and set light gray colour */
# ifdef SMALL
	PUTCHAR(147); /* clear screen */
	PUTCHAR(155); /* gray 3 */
# else
	clrscr();
	textcolor(COLOR_GRAY3);
# endif
#endif

#ifdef SMALL
	PUTS("The Potion\n"
	     "Copyright 2002-2003 Peter Karlsson.\n"
	     "http://www.softwolves.pp.se/cbm/\n");
#else
	putstring("The Potion - a simple adventure\n"
	          "==========\n\n"
	          "Copyright 2002-2003 Peter Karlsson.\n"
	          "A Softwolves Software Release.\n"
	          "http://www.softwolves.pp.se/cbm/\n\n"
	          "This program comes with ABSOLUTELY NO WARRANTY. This is free "
	          "software, and you are welcome to redistribute it under "
	          "certain conditions; type \"license\" for details.\n"
	          );
#endif
}
