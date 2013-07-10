#include <stdio.h>

/* Change this if you want a different number of spaces for `\t' */
#define TAB_SPACES	8

int
main(argc, argv)
	int argc;
	char **argv;
{
	int i, j, col = 0;
	char *ch;

	for (i = 1; i < argc; i++) {
		ch = argv[i];
		
		if (i > 1) {
			putchar(' ');
			col++;
		}

		while (*ch) {
			if (*ch == '\\' && *(ch + 1) == 't') {
				j = col + TAB_SPACES - (col % TAB_SPACES);
				for (; col < j; col++)
					putchar(' ');
				ch += 2;
			}
			else {
				putchar(*ch++);
				col++;
			}
		}
	}
	return 0;
}
