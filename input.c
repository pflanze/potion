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

static const char* const nargs_help[NUMNARGS] = {
    "without an object",
    "optionally with an object",
    "with an object",
};

static const struct verb_s verbs[] =
{
#define VERB_S(str, const) { sizeof(str)-1, str, const }
    VERB_S("n", V_N),
    VERB_S("s", V_S),
    VERB_S("e", V_E),
    VERB_S("w", V_W),
    VERB_S("i", V_I),
    VERB_S("inventory", V_I),
    VERB_S("take", V_TAKE),
    VERB_S("pick up", V_TAKE),
    VERB_S("lift", V_LIFT),
    VERB_S("read", V_READ),
    VERB_S("talk to", V_TALKTO),
    VERB_S("unlock", V_UNLOCK),
    VERB_S("open", V_UNLOCK),
    VERB_S("turn", V_TURN),
    VERB_S("hit", V_HIT),
    VERB_S("punch", V_HIT),
    VERB_S("kick", V_HIT),
    VERB_S("kill", V_KILL), /* but does not pass object to kill() */
    VERB_S("pour", V_POUR),
    VERB_S("empty", V_POUR),
    VERB_S("drink", V_DRINK),
    VERB_S("throw", V_THROW),
    VERB_S("look at", V_LOOK),
    VERB_S("look", V_LOOK),
    VERB_S("examine", V_LOOK),
    VERB_S("drop", V_DROP),
#ifndef SMALL
    VERB_S("license", V_LICENSE),
    VERB_S("help", V_HELP),
    VERB_S("?", V_HELP),
#endif
    VERB_S("quit", V_QUIT),
    VERB_S("exit", V_QUIT),
#undef VERB_S
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
    unsigned char inargs, i, verbnum, last_verbnum;
    PUTS("You can say the following:\n");
    for (inargs=0; inargs<NUMNARGS; inargs++) {
        prints("- ");
        prints(nargs_help[inargs]);
        prints(":\n\n  ");
        last_verbnum = 255;
        for (i=0; i<VERBS; i++) {
            verbnum= verbs[i].verbnum;
            if (v_nargs[verbnum - 1] == inargs) {
                if (last_verbnum == 255) {
                    /* none seen, print nothing */
                } else if (last_verbnum == verbnum) {
                    prints(" / ");
                } else {
                    prints(", ");
                }
                last_verbnum = verbnum;
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
