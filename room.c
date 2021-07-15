#include <stdio.h>

#include "game.h"
#include "object.h"
#include "output.h"

void printroom(void)
{
	NR unsigned char i;
	NR BOOL seen;
	NR const char *s;
	NR unsigned char mapdata;
	NR signed char *object;

	seen = FALSE;
	mapdata = gamedata->map[gamedata->room - 1];

	/* Print room description */
	switch (gamedata->room)
	{
		case 1:  case 2:  case 3:
		case 7:  case 8:  case 9:
		case 13: case 14: case 15:
		case 30:
#ifdef SMALL
			s = "You are in a forest.";
#else
			s = "You are in a forest. There are trees in every direction "
			    "as far as the eye can see.";
#endif
			break;

		case 4: case 10:
		case 16: case 18:
#ifdef SMALL
			s = "You are on a big plain.";
#else
			s = "You are standing on a big plain. "
			    "You hear the distant hum of motors from somewhere.";
#endif
			break;

		case 12:
#ifdef SMALL
			s = "You are at a recycling station.";
#else
			s = "You are standing on a big plain, next to a recycling "
			    "station.";
#endif
			break;

		case 5: case 11: case 17:
		case 23: case 24:
			switch (gamedata->roadvisit)
			{
			case 0:
			case 1:
				++ gamedata->roadvisit;
			case 3:
#ifdef SMALL
				s = "You are standing on an autobahn.";
#else
				s = "You are standing on an autobahn. "
				    "Several cars are passing passing by.";
#endif
				break;

			case 2:
#ifdef SMALL
				s = "You are standing on an autobahn.\n"
				    "A skateboarder passes by, grabbing a\n"
				    "Coca Cola from a nearby car.";
#else
				s = "You are standing on an autobahn. "
				    "Suddenly you see a skateboarder pass by, grabbing a "
				    "bottle of Coca Cola from a nearby car.";
#endif
				++ gamedata->roadvisit;
				gamedata->objects[O_COKE] = gamedata->room;
				break;
			}
			break;

		case 6:
#ifdef SMALL
			s = "You are standing by the sea.";
#else
			s = "You are standing on the sea shore. The sea stretches out "
			    "as far as the eye can see to the north and to the east.";
#endif
			break;

		case 19: case 25:
#ifdef SMALL
			s = "You are in the scientist's lab.";
#else
			s = "You are inside the scientist's lab. You notice that he "
			    "doesn't seem to put much energy in keeping it tidy.";
#endif
			break;

		case 20: case 21: case 26: case 27:
#ifdef SMALL
			s = "You are inside a big fortress.";
#else
			s = "You are standing inside a big fortress. There are some "
			    "spider webs in the corners, and a layer of dust on "
			    "pretty much everything.";
#endif
			break;

		case 22:
			if (!(unsigned char) (mapdata & EXIT_WEST))
			{
#ifdef SMALL
				s = "You are standing beside a moat. You\n"
				    "must lower the drawbridge to pass.";
#else
				s = "You are standing beside a big moat. There is no way "
				    "to pass over it without lowering the drawbridge.";
#endif
			}
			else
			{
#ifdef SMALL
				s = "You are standing beside a drawbridge.";
#else
				s = "You are standing beside a drawbridge leading to "
				    "a big fortress.";
#endif
			}
			break;

		case 28: case 29:
			s = "You are standing on a dump. It stinks!";
			break;

		default:
#ifdef SMALL
			s = "?ERROR";
#else
			s = "Something is very wrong!";
#endif
			break;
	}

#ifdef SMALL
	PUTS(s);
#else
	putstring(s);
#endif

	/* Print room contents */
	object = gamedata->objects;
	for (i = 0; i < OBJECTS; ++ i, ++ object)
	{
		if (*object == gamedata->room)
		{
			if (!seen)
			{
				PUTS("You see:");
				seen = TRUE;
			}
			printobject(i);

			if (O_SCIENTIST == i && !gamedata->seenscientist)
			{
				PUTS("He says: \"What are you doing here?\"");
				gamedata->seenscientist = TRUE;
			}
		}
	}

	/* Print exits */
	PUTS("Available exits:");
	if ((unsigned char) (mapdata & EXIT_NORTH) != 0)
		PUTCHAR('N');
	if ((unsigned char) (mapdata & EXIT_SOUTH) != 0)
		PUTCHAR('S');
	if ((unsigned char) (mapdata & EXIT_EAST) != 0)
		PUTCHAR('E');
	if ((unsigned char) (mapdata & EXIT_WEST) != 0)
		PUTCHAR('W');
	PUTCHAR('\n');
}
