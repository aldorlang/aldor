/* older24h.c - 08/30/89	Florian Bundschuh */
/* This program expects one parameter, a file name.
 *	Status returns:
 *		0 - file has not been modified in the last 24 hours
 *		1 - file has been modified in the last 24 hours
 *		2 - some error
 */

#define PERIOD 24*60*60
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

main(argc, argv)
int argc;
char *argv[];
{
	char file[MAXPATHLEN];
	int ret;
	struct stat buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: older24h filename\n");
		exit(2);
	}

	strcpy(file, argv[1]);
	if ((ret = stat(file, &buf)) != 0) {
		fprintf(stderr, "Access to file \"%s\" failed\n", file);
		exit(2);
	}

	if (buf.st_mtime < time(0) - PERIOD)
		exit(0);
	else
		exit(1);
}
