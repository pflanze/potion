#include <stdio.h>

#include "action.h"
#include "object.h"
#include "perform.h"
#include "input.h"
#include "game.h"
#include "output.h"

void cannotdothat(void)
{
	PUTS("You cannot do that!");
}

void nothing(void)
{
	PUTS("Nothing.");
}

unsigned char move(unsigned char verb)
{
	unsigned char bitmap;
	signed char offset;

	/* Seen scientist? */
	if (gamedata->seenscientist)
	{
		PUTS("The scientist captures you.");
		return GAME_LOST;
	}

	/* FIXME: Crossing autobahn? */

	/* Check which movement is requested */
	switch (verb)
	{
		case V_N: bitmap = EXIT_NORTH; offset = -6; break;
		case V_S: bitmap = EXIT_SOUTH; offset =  6; break;
		case V_E: bitmap = EXIT_EAST;  offset =  1; break;
		case V_W: bitmap = EXIT_WEST;  offset = -1; break;
	}

	/* Check if movement is allowed in that direction */
	if (gamedata->map[gamedata->room - 1] & bitmap)
	{
		gamedata->room += offset;
	}
	else
	{
		PUTS("You cannot go that way.");
		return GAME_CONTINUES;
	}

	/* Check if we are carrying both bottles */
#ifdef SMALL
	__asm__("lda %w", (unsigned int) gamedata->inventory);
	__asm__("ldx %w", (unsigned int) &gamedata->inventory[1]);
	__asm__("cmp #%b", (unsigned char) O_ELIXIR);
	__asm__("bne checktwo");
	__asm__("cpx #%b", (unsigned char) O_COKE);
	__asm__("beq haveboth");
	__asm__("checktwo: cmp #%b", (unsigned char) O_COKE);
	__asm__("bne notboth");
	__asm__("cpx #%b", (unsigned char) O_ELIXIR);
	__asm__("bne notboth");
	__asm__("haveboth:");
#else
	if ((O_ELIXIR == gamedata->inventory[0] && O_COKE == gamedata->inventory[1])
	    ||
	    (O_ELIXIR == gamedata->inventory[1] && O_COKE == gamedata->inventory[0]))
#endif
	{
		switch (++ gamedata->bothbottles)
		{
		case 1:
#ifdef SMALL
			PUTS("The potion is heating.");
#else
			putstring("The potion is starting to heat up.");
#endif
			break;

		case 5:
#ifdef SMALL
			PUTS("The potion is reacting with the Coke.");
#else
			putstring("Some kind kind of reaction seems to be happening "
			          "between the potion and what is left of the Coke.");
#endif
			break;

		case 10:
#ifdef SMALL
			PUTS("The bottles are about to explode!");
#else
			putstring("You better get rid of one of the bottles before "
			          "the potion explodes!");
#endif
			break;

		case 12:
#ifdef SMALL
			PUTS("The potion explodes in your face!");
#else
			putstring("The potion explodes in your face!");
#endif
			return GAME_LOST;
		}
	}
#ifdef SMALL
	__asm__("jmp skip");
	__asm__("notboth:");
#else
	else
#endif
	{
		/* Reset counter */
		gamedata->bothbottles = 0;
	}
#ifdef SMALL
	__asm__("skip:");
#endif

	return GAME_CONTINUES;
}

unsigned char inventory(void)
{
	NR BOOL hasanything;
#ifndef SMALL
	NR unsigned char i;
	NR signed char *inventory;
#endif

	hasanything = FALSE;

#ifdef SMALL
	PUTS("Inventory:");

	if (gamedata->inventory[0] != -1)
	{
		printobject(gamedata->inventory[0]);
		hasanything = TRUE;
	}
	if (gamedata->inventory[1] != -1)
	{
		printobject(gamedata->inventory[1]);
		hasanything = TRUE;
	}
#else
	inventory = gamedata->inventory;

	PUTS("You are carrying:");
	for (i = INVENTORY; i; -- i, ++ inventory)
		if (*inventory != -1)
		{
			printobject(*inventory);
			hasanything = TRUE;
		}
#endif

	if (!hasanything)
		nothing();

	return GAME_CONTINUES;
}

unsigned char take(unsigned char object)
{
	switch (object)
	{
		case O_PILL:
			/* Something special happens when we take the pill */
			gamedata->pilleaten = TRUE;
			gamedata->objects[O_PILL] = -1;
			PUTS("You understand.");
			break;

		case O_STONE_PILL:
			/* Finds pill, if it's not taken already */
			if (!gamedata->pilleaten && -1 == gamedata->objects[O_PILL])
			{
#ifdef SMALL
				PUTS("You find a pill.");
#else
				PUTS("You found a small pill beneath it.");
#endif
				gamedata->objects[O_PILL] = gamedata->room;
				break;
			}
			/* Else fall through */

		default:
			if (movable(object))
			{
				if (addinventory(object))
					PUTS("Taken.");
			}
			else
			{
				cannotdothat();
			}
			break;
	}
	return GAME_CONTINUES;
}

unsigned char lift(unsigned char object)
{
	switch (object)
	{
		case O_STONE_PILL:
			if (!gamedata->pilleaten)
			{
				take(object);
				break;
			}
			/* else fall through */

		default:
			nothing();
			break;
	}

	return GAME_CONTINUES;
}

unsigned char read(unsigned char object)
{
	switch (object)
	{
		case O_STONE_MISSION:
#ifdef SMALL
			PUTS("\"Save us from the evil scientist.\n"
			     " Mix his evil drug with salt water.\"");
#else
			putstring("It says:\n"
			          "\"Save us from the evil scientist. "
			          "Mix his evil drug with salt water. "
			          "That is your mission.\"");
#endif
			break;

		default:
			nothing();
	}

	return GAME_CONTINUES;
}

unsigned char talkto(unsigned char object)
{
	switch (object)
	{
		case O_SCIENTIST:
#ifdef SMALL
			PUTS("\"You will die with the rest!\"");
#else
			putstring("The scientist says:\n"
			          "\"You will die with the rest!\" "
			          "and laughs frantically.\n");
#endif
			break;

		case O_SQUIRREL:
			if (gamedata->pilleaten)
			{
#ifdef SMALL
				PUTS("\"You can now exit the forest\"");
#else
				putstring("The squirrel says:\n"
				          "\"You can now exit the forest\"");
#endif
				gamedata->map[3 - 1] |= EXIT_EAST;
				break;
			}
			/* else fall through */

		default:			
			cannotdothat();
	}

	return GAME_CONTINUES;
}

unsigned char unlock(unsigned char object)
{
	/* Check if we have the key */
	NR BOOL havekey;
#ifndef SMALL
	NR unsigned char i;
#endif

	havekey = FALSE;

#ifdef SMALL
	__asm__("lda %w", (unsigned int) gamedata->inventory);
	__asm__("cmp #%b", (unsigned char) O_KEY);
	__asm__("bne checktwo");
	++ havekey;
	__asm__("checktwo: lda %w", (unsigned int) &gamedata->inventory[1]);
	__asm__("cmp #%b", (unsigned char) O_KEY);
	__asm__("bne out");
	++ havekey;
	__asm__("out:");
#else
	for (i = 0; i < INVENTORY; ++ i)
		if (O_KEY == gamedata->inventory[i])
			havekey = TRUE;
#endif

	if (havekey)
	{
		switch (object)
		{
			case O_COTTAGE:
#ifdef SMALL
				PUTS("You find a switch.");
#else
				PUTS("You find a switch inside.");
#endif
				gamedata->objects[O_SWITCH] = gamedata->room;
				return GAME_CONTINUES;
			
			case O_DOOR:
				PUTS("It opens revealing a laboratory.");
				gamedata->map[20 - 1] |= EXIT_WEST;
				return GAME_CONTINUES;
		}
	}

	cannotdothat();
	return GAME_CONTINUES;
}

unsigned char turn(unsigned char object)
{
	switch (object)
	{
		case O_SWITCH:
			PUTS("Thud.");
			gamedata->map[22 - 1] |= EXIT_WEST;
			break;

		default:
			cannotdothat();
			break;
	}

	return GAME_CONTINUES;
}

unsigned char hit(unsigned char object)
{
	switch (object)
	{
		case O_SQUIRREL:
			/* If we hit the squirrel, we kill it. */
			PUTS("You kill the squirrel.");
			return GAME_LOST;

		case O_SCIENTIST:
			/* Clear the seen scientist flag */
			gamedata->seenscientist = FALSE;
			/* Remove scientist from game */
			gamedata->objects[O_SCIENTIST] = -1;

			PUTS("He will not bother you again.");
			break;

		default:
			cannotdothat();
			break;
	}

	return GAME_CONTINUES;
}

unsigned char kill(void)
{
#ifdef SMALL
	PUTS("This is a family game.");
#else
	putstring("No strong violence please. This is a family game.");
#endif
	return GAME_CONTINUES;
}

unsigned char pour(unsigned char object)
{
	switch (object)
	{
		case O_ELIXIR:
			/* The game object is to mix the potion with salt
			 * water. This can only be done by pouring it into
			 * the sea.
			 */
			 switch (gamedata->room)
			 {
			 	case 6:
#ifdef SMALL
					PUTS("You neutralised it.");
#else
					PUTS("The potion has been neutralised.");
#endif
					return GAME_WON;

				default:
#ifdef SMALL
					PUTS("It poisons the land.");
#else
					PUTS("The potion poisons the land.");
#endif
					return GAME_LOST;
			}
			break;

		case O_COKE:
#ifdef SMALL
			PUTS("It's empty.");
#else
			PUTS("The bottle is empty.");
#endif
			break;

		default:
			cannotdothat();
			break;
	}

	return GAME_CONTINUES;
}

unsigned char drink(unsigned char object)
{
	switch (object)
	{
		case O_ELIXIR:
			/* Can only drink the elixir, and we lose if we do it */
			PUTS("You die instantly.");
			return GAME_LOST;

		case O_COKE:
			/* It's empty */
			return pour(O_COKE);
			break;

		default:
			cannotdothat();
			return GAME_CONTINUES;
	}
}

unsigned char throw(unsigned char object)
{
	if ((object <= O_LAST_STONE || O_COKE == object) &&
	    9 == gamedata->room)
	{
		return hit(O_SQUIRREL);
	}
	else
	{
		return drop(object);
	}
}

unsigned char look(unsigned char object)
{
	switch (object)
	{
		case O_STONE_MISSION:
			return read(object);

		case O_STONE_PILL:
			if (!gamedata->pilleaten && -1 == gamedata->objects[O_PILL])
				return take(object);
			/* Else fall through */

		default:
			nothing();
			return GAME_CONTINUES;
	}
}

unsigned char drop(unsigned char object)
{
	switch (object)
	{
		case O_COKE:
			/* Can only recycle the Cola bottle once we've taken it */
			switch (gamedata->room)
			{
			case 12:
				if (dropinventory(O_COKE))
				{
#ifdef SMALL
					PUTS("Thank you for recycling!");
#else
					putstring("You recycle the bottle. Thank you for being "
					          "kind to the nature!");
#endif
					gamedata->objects[O_COKE] = -1; /* Remove from game */
					break;
				}
				/* else fall through */

			case 28: case 29: /* Dump */
#ifndef SMALL
				putstring("Throwing away a perfectly recyclable bottle? "
				          "No way!");
				break;
#endif
				/* Fall through for small build */

			case 1:  case 2:  case 3:
			case 7:  case 8:  case 9:
			case 13: case 14: case 15:
			case 30: /* Forest */
#ifndef SMALL
				PUTS("Someone might get hurt!");
				break;
#endif
				/* Fall through for small build */

			default:
#ifdef SMALL
				PUTS("Please recyle.");
#else
				PUTS("Please recycle the bottle.");
#endif
				break;
			}
			break;

		case O_ELIXIR:
			if (gamedata->bothbottles)
			{
#ifdef SMALL
				PUTS("You do not dare to.");
#else
				putstring("You dare not drop the potion in its current "
				          "state!");
#endif
				break;
			}
			/* else fall through */

		default:
			if (dropinventory(object))
				PUTS("Dropped.");
	}

	return GAME_CONTINUES;
}

#ifndef SMALL
unsigned char license(void)
{
	putstring("This program is free software; you can redistribute it "
              "and/or modify it under the terms of the GNU General Public "
              "License as published by the Free Software Foundation, "
              "version 2.\n\n"
              "This program is distributed in the hope that it will be "
              "useful, but WITHOUT ANY WARRANTY; without even the implied "
              "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR "
              "PURPOSE.  See the GNU General Public License for more "
              "details."
#ifndef __C64__
              "\n\n"
              "You should have received a copy of the GNU General Public "
              "License along with this program; if not, write to the Free "
              "Software Foundation, Inc., 59 Temple Place, Suite 330, "
              "Boston, MA  02111-1307  USA"
#endif
              );

	return GAME_CONTINUES;
}
#endif

unsigned char quit(void)
{
#ifdef SMALL
	PUTS("You hear the evil scientist laughing\n"
	     "mercilessly.");
#else
	putstring("You leave for your home country. In the distance, you "
	          "can hear the evil scientist laughing mercilessly as he "
	          "unleashes his horrible magical potion on the land.");
#endif
	return GAME_LOST;
}
