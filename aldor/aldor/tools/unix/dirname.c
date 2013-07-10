/*
 * dirname /tmp/x --> /tmp
 */

#include <stdio.h>
#include <string.h>

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	int	i, n;
	char	*p;

	p = (argc > 1) ? argv[1] : "./xx";

	/* Count slashes. */
	for (n = 0, i = 0; p[i]; i++) if (p[i] == '/') n++;

	if (n == 0)
		printf(".");
	else if (n == 1 && p[0] == '/')
		printf("/");
	else {
		/* Print upto, but not including, last slash. */
		for (i = strlen(p)-1; i >= 0; i--)
			if (p[i] == '/') { n = i; break; }
		for (i = 0; i < n; i++)
			putchar(p[i]);
	}

	printf("\n");
	return 0;
}
