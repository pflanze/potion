#include <stdio.h>
#include <string.h>

#include "input.h"
#include "game.h"
#include "output.h"

struct verb_s
{
	const unsigned char len;
	const char *s;
	unsigned char verbnum;
};

static const struct verb_s verbs[] =
{
#define ENTRY(str, const) { sizeof(str)-1, str, const }
    ENTRY("n", V_N),
    ENTRY("s", V_S),
    ENTRY("e", V_E),
    ENTRY("w", V_W),
    ENTRY("i", V_I),
    ENTRY("take", V_TAKE),
    ENTRY("pick up", V_TAKE),
    ENTRY("lift", V_LIFT),
    ENTRY("read", V_READ),
    ENTRY("talk to", V_TALKTO),
    ENTRY("unlock", V_UNLOCK),
    ENTRY("open", V_UNLOCK),
    ENTRY("turn", V_TURN),
    ENTRY("hit", V_HIT),
    ENTRY("punch", V_HIT),
    ENTRY("kick", V_HIT),
    ENTRY("kill", V_KILL),
    ENTRY("pour", V_POUR),
    ENTRY("empty", V_POUR),
    ENTRY("drink", V_DRINK),
    ENTRY("throw", V_THROW),
    ENTRY("look at", V_LOOK),
    ENTRY("look", V_LOOK),
    ENTRY("examine", V_LOOK),
    ENTRY("drop", V_DROP),
#ifndef SMALL
    ENTRY("license", V_LICENSE),
    ENTRY("help", V_HELP),
    ENTRY("?", V_HELP),
#endif
    ENTRY("quit", V_QUIT),
    ENTRY("exit", V_QUIT),
#undef ENTRY
};

#define VERBS sizeof(verbs)/sizeof(struct verb_s)

void print_help(void) {
    unsigned char i;
    PUTS("You can say the following:\n");
    for (i=0; i<VERBS; i++) {
        if (i != 0) {
            PRINTS(", ");
        }
        PRINTS(verbs[i].s);
    }
    PRINTS("\n\n"); 
}


#ifdef __C64__
static void __fastcall__ getline(void)
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
		PUTS("I do not understand. Say '?' or 'help' for help.");
#endif
	}
}
