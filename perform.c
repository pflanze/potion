#include <stdio.h>

#include "perform.h"
#include "input.h"
#include "action.h"
#include "object.h"
#include "output.h"
#include "room.h"

unsigned char perform(unsigned char verb)
{
	signed char object;

	/* Verbs that do not require an object */
	switch (verb)
	{
		case V_N:
		case V_S:
		case V_E:
		case V_W:
			return move(verb);

		case V_I:
			return inventory();

#ifndef SMALL
		case V_LICENSE:
			return license();
                case V_HELP:
                        print_help();
                        return GAME_CONTINUES;
#endif

		case V_QUIT:
			return quit();
	}

	/* Other words require an object */
	object = object_string
		? objectfromword(object_string, verb != V_TAKE)
		: -1;

	if (-1 == object)
	{
		if (V_LOOK == verb)
		{
			printroom();
		}
		else
		{
#ifdef SMALL
			PUTS("What?");
#else
			PUTS("I do not see that here.");
#endif
		}
		return GAME_CONTINUES;
	}

	switch (verb)
	{
		case V_TAKE:
			return take((unsigned char) object);

		case V_LIFT:
			return lift((unsigned char) object);

		case V_READ:
			return read((unsigned char) object);

		case V_TALKTO:
			return talkto((unsigned char) object);

		case V_UNLOCK:
			return unlock((unsigned char) object);

		case V_TURN:
			return turn((unsigned char) object);

		case V_HIT:
			return hit((unsigned char) object);

		case V_KILL:
			return kill();

		case V_POUR:
			return pour((unsigned char) object);

		case V_DRINK:
			return drink((unsigned char) object);

		case V_THROW:
			return throw((unsigned char) object);

		case V_LOOK:
			return look((unsigned char) object);

		case V_DROP:
			return drop((unsigned char) object);
	}

#ifndef SMALL
	PUTS("Say what?");
#endif

	/* This should never happen */
	return GAME_CONTINUES;
}
