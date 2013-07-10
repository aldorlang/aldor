/*
 * skimenum fname enum_name1 enum_name2 ... 
 *
 * Extract enumeration values from C source.
 * SMW Oct/90 -- Created
 * SMW Jan/94 -- Split engine from main routine.
 *
 * A file name of "-" is taken to mean standard input.
 *
 * Each output line contains the enum name, the item name and the item value.
 *
 * Only a limited form of explicit values are supported: 
 *   integers and previous enumeration items.
 *
 * The enumeration definition can have any desired layout, including comments.
 *
 * E.g. If a file contains
 *
 *   enum foolish {
 *	fstart = 10, fa, fb,
 *   fc, fd = 6, fe, ff = fstart };
 *
 * then the output will contain
 *
 *   foolish fstart 10
 *   foolish fa 11
 *   foolish fb 12
 *   foolish fc 13
 *   foolish fd 6
 *   foolish fe 7
 *   foolish ff 10
 */

#include <stdio.h>
#include "cenum.h"

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	EnumItem	el, t;

	if (argc < 3) {
		printf("Usage: skimenums fname enum_name1 enum_name2 ...\n");
		return 1;
	}

	el = skimEnums(argv[1], argc-2, argv+2);

	for (t = el; t; t = t->next)
		printf("%s %s %d\n", t->ename, t->id, t->value);

	skimFree(el);
	return 0;
}
