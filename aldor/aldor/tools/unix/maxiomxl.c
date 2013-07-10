/* maldor.c - Used to suid'ly execute .../maldor.sh */
/* FB 08/15/89 */
/* SMW 11/93 to work in under ALDORSERV. */

#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>

extern char *	getenv();

main(argc, argv)
	int argc;
	char *argv[];
{
	char path[MAXPATHLEN];

	argv[0] = "maldor.sh";
	strcpy(path, getenv("ALDORSERV"));
	strcat(path, "/mabin/maldor.sh");
	execv(path, argv);
	fprintf(stderr,
	        "\nCannot exec maintain, check the ALDORSERV variable.\n");
	exit(EX_OSERR);
}
