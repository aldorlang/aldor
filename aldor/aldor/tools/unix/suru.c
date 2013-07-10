/* suru.c - Set euid to ruid and execute argument vector. */
/* FB 08/15/89 */

main(argc, argv)
	int argc;
	char *argv[];
{
	setuid(getuid());
	execvp(argv[1], &argv[1]);
}
