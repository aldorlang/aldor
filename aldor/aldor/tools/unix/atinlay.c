/*
 * atinlay < infile > outfile
 *
 * Translate a message file to Pig Latin. (Not sure about rules.)
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int
main()
{
	int	c;

	for (;;) {
		c = getchar();
		if (c == EOF) return 0;

		if (c == '$') {
			putchar(c);
			for (;;) {
				c = getchar();
				if (c == EOF) return 0;
				putchar(c);
				if (c == '\n') break;
			}
		}
		else if (c == '%') {
			putchar(c);
			do {
				c = getchar();
				if (c == EOF) return 0;
				putchar(c);
			} while (strchr("-+# 0123456789.hlL", c));
		}
		else if (c == '\\') {
			putchar(c);
			c = getchar();
			if (c == EOF) return 0;
			putchar(c);
		}
		else if (isalpha(c)) {
			char	w[5000];
			int	i;
			int	CAPS = 1, Caps = 1;

			for (i = 0; isalpha(c); i++) {
				if (isupper(c)) { if (i != 0) Caps = 0; }
				else { if (i == 0) Caps = 0; CAPS = 0; }
				w[i] = c;
				c = getchar();
			}
			ungetc(c, stdin);
			w[i] = 0;

			/* Find first vowel */
			for (i = 0; w[i] && !strchr("aeiouAEIOU", w[i]); i++)
				;
			/* Pigify */
			if (i == 0 || w[i] == 0)
				printf(CAPS ? "%sIDAY" : "%say", w);
			else {
				if (Caps || CAPS) w[i] = toupper(w[i]);
				printf("%s", w+i);

				if (Caps) w[0] = tolower(w[0]);
				w[i] = 0;
				printf(CAPS ? "%sAY" : "%say", w);
			}
		}
		else {
			putchar(c);
		}
	}
}
