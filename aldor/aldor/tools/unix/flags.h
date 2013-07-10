/****************************** flags.h ******************************/

#define existFlag(ac, ag, flag)	(getFlag(ac, ag, flag) >= 0)

/* returns the nth string argument */
char *getNthArgument(int ac, char **ag, int pos);

/* returns the numerical value of a given flag */
int getFlag(int ac, char **ag, char flag);

/* Returns the string value of a flag, or NULL if not there */
char *getFlagName(int ac, char **ag, char flag);

