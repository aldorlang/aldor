/*
 * cenum.h
 *
 * Extract enumeration values from C source.
 * SMW Oct/90	-- Created
 * SMW Jan/94	-- Split engine from main routine.
 *
 *
 * skimEnums
 * =========
 * This function takes as arguments an input file name and
 * a count+vector of enumeration type names.  
 *
 * The function scans the file for the named enumerations and produces
 * a linked list of enumeration values.  Each link contains the name of
 * an enumeration type as well as the name and value of an enumerated constant.
 *
 * Only a limited form of explicit values are supported for C source files: 
 * integers and previous enumeration items.
 *
 * The enumeration definition can have any desired layout, including comments.
 *
 * E.g. If a file contains
 *
 *   enum foolish {
 *	fstart = 10, fa, fb,
 *   fc, fd = 6, fe, ff = fstart };
 *
 * then the result list will contain
 *
 *  { "foolish", "fstart", 10, *}
 *  { "foolish", "fa",     11, *}
 *  { "foolish", "fb",     12, *}
 *  { "foolish", "fc",     13, *}
 *  { "foolish", "fd",     6,  *}
 *  { "foolish", "fe",     7,  *}
 *  { "foolish", "ff",     10, *}
 *
 * skimFree
 * ========
 * This function deallocates a structure produced by skimEnums.
 *
 * skimNConcat
 * ===========
 * Joins enumeration lists distructively.
 */

struct enumItem {
	char		*ename; 	/* Name of the enum type. */
	char		*id;		/* Name of the enum element. */
	int		value;		/* Integer value of the enum element. */
	struct enumItem *next;
};

typedef struct enumItem	*EnumItem;

extern EnumItem	skimEnums	(/* char *fname, int enamec, char **enamev */);
extern void	skimFree  	(/* EnumItem */);
extern EnumItem skimNConcat	(/* EnumItem, EnumItem */);
