#include <stdio.h>
#include <string.h>

#include "input.h"
#include "game.h"
#include "output.h"

#ifdef SMALL
# define VERBS 27
#else
# define VERBS 29
#endif

static const struct verb_s
{
	const unsigned char len;
	const char *s;
	unsigned char verbnum;
} verbs[VERBS] =
{
	{ 1, "n", V_N },
	{ 1, "s", V_S },
	{ 1, "e", V_E },
	{ 1, "w", V_W },
	{ 1, "i", V_I },
	{ 4, "take", V_TAKE },
	{ 7, "pick up", V_TAKE },
	{ 4, "lift", V_LIFT },
	{ 4, "read", V_READ },
	{ 7, "talk to", V_TALKTO },
	{ 6, "unlock", V_UNLOCK },
	{ 4, "open", V_UNLOCK },
	{ 4, "turn", V_TURN },
	{ 3, "hit", V_HIT },
	{ 5, "punch", V_HIT },
	{ 4, "kick", V_HIT },
	{ 4, "kill", V_KILL },
	{ 4, "pour", V_POUR },
	{ 5, "empty", V_POUR },
	{ 5, "drink", V_DRINK },
	{ 5, "throw", V_THROW },
	{ 7, "look at", V_LOOK },
	{ 4, "look", V_LOOK },
	{ 7, "examine", V_LOOK },
	{ 4, "drop", V_DROP },
#ifndef SMALL
	{ 7, "license", V_LICENSE },
	{ 7, "licence", V_LICENSE },
#endif
	{ 4, "quit", V_QUIT },
	{ 4, "exit", V_QUIT },
};

#ifdef __C64__
void __fastcall__ getline(void)
{
#define CHRIN 0xFFCF

	__asm__("ldy #$FF");
	__asm__("chrinloop: iny");
	__asm__("jsr %w", CHRIN);
	__asm__("sta %w,y", (unsigned int) input);
	__asm__("cmp #%b", '\n');
	__asm__("bne chrinloop");
	__asm__("lda #0");
	__asm__("sta %w,y", (unsigned int) input);
}
#endif

unsigned char getinput(void)
{
	while (1)
	{
		NR unsigned char i;
		NR const struct verb_s *verb;

#ifdef __C64__
		PUTS("Your turn?");
		getline();
#else
		fputs("Your turn? ", stdout);
		fgets(input, 1024, stdin);
		input[1023] = 0;
		input[strlen(input) - 1] = 0;
#endif
		PUTCHAR('\n');

		verb = verbs;
		for (i = VERBS; i; -- i, ++ verb)
		{
			NR unsigned char n;
			NR char *p;

			n = verb->len; /* Verb length */
			p = input + n; /* First character after verb */
			if (!strncmp(input, verb->s, n) &&
			    (!*p || ' ' == *p))
			{
				if (' ' == *p)
				{
					object_string = p + 1;
				}
				else
				{
					object_string = NULL;
				}
				return verb->verbnum;
			}
		}

#ifdef SMALL
		PUTS("What?");
#else
		PUTS("I do not understand.");
#endif
	}
}
