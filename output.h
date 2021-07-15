#ifndef OUTPUT_H
#define OUTPUT_H

#ifdef SMALL

# define CHROUT 0xFFD2
# define PUTCHAR(c) { __asm__("lda #%b", (unsigned char) c); __asm__("jsr %w", CHROUT); }
# define PUTS(s) myputs(s)
void myputs(const unsigned char *s);

#else

/* Print a string on the screen, wrapping it properly.
 * Like puts() it adds a final line break. */
void putstring(const char *s);

# define PUTCHAR(c) putchar(c)
# define PUTS(s) puts(s)

#endif

#endif
