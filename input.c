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

#define NARGS_0             0
#define NARGS_OPTIONAL      1
#define NARGS_1             2

#define NUMNARGS 3

static const char* nargs_help[NUMNARGS] = {
    "without an object",
    "optionally with an object",
    "with an object",
};

static const struct verb_s verbs[] =
{
#define ENTRY(str, const) { sizeof(str)-1, str, const }
    ENTRY("n", V_N),
    ENTRY("s", V_S),
    ENTRY("e", V_E),
    ENTRY("w", V_W),
    ENTRY("i", V_I),
    ENTRY("inventory", V_I),
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
    ENTRY("kill", V_KILL), /* but does not pass object to kill() */
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

#ifndef SMALL
static const v_nargs[NUMV] = {
#define T(const, nargs) nargs
    T(V_N, NARGS_0),
    T(V_S, NARGS_0),
    T(V_E, NARGS_0),
    T(V_W, NARGS_0),
    T(V_I, NARGS_0),
    T(V_TAKE, NARGS_1),
    T(V_LIFT, NARGS_1),
    T(V_READ, NARGS_1),
    T(V_TALKTO, NARGS_1),
    T(V_UNLOCK, NARGS_1),
    T(V_TURN, NARGS_1),
    T(V_HIT, NARGS_1),
    T(V_KILL, NARGS_1), /* but does not pass object to kill() */
    T(V_POUR, NARGS_1),
    T(V_DRINK, NARGS_1),
    T(V_THROW, NARGS_1),
    T(V_LOOK, NARGS_OPTIONAL),
    T(V_DROP, NARGS_1),
    T(V_LICENSE, NARGS_0),
    T(V_HELP, NARGS_0),
    T(V_QUIT, NARGS_0),
#undef T
};
#endif

void print_help(void) {
    unsigned char inargs, i, is_first;
    PUTS("You can say the following:\n");
    for (inargs=0; inargs<NUMNARGS; inargs++) {
        prints("- ");
        prints(nargs_help[inargs]);
        prints(":\n\n  ");
        is_first = 1;
        for (i=0; i<VERBS; i++) {
            if (v_nargs[verbs[i].verbnum - 1] == inargs) {
                if (is_first) {
                    is_first = 0;
                } else {
                    prints(", ");
                }
                prints(verbs[i].s);
            }
        }
        prints("\n\n");
    }
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
