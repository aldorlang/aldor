/* realuser.c -- 08/30/89	Florian Bundschuh */

/* Print the real user id on standard output */

#include <pwd.h>
struct	passwd *getpwuid();

main()
{
	register struct passwd *pp;

	pp = getpwuid(getuid());
	if (pp == 0) {
		printf("Intruder alert.\n");
		exit(1);
	}
	printf("%s\n", pp->pw_name);
}

