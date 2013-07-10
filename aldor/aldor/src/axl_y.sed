# Fixups for the machine generated axl_y.c file.

#
# Put braces around the token info initializers.
#
s|	"_YY_\(.*\),$|	{ "_YY_\1 },|g
s|	"-unknown-\(.*\)$|	{ "-unknown-\1 }|g

#
# Conditionalize away unused labels.
#
/^yyerrlab:$/s//#ifdef UNUSED_LABELS\
&\
#endif \/* UNUSED_LABELS *\//

/^yynewerror:$/s//#ifdef UNUSED_LABELS\
&\
#endif \/* UNUSED_LABELS *\//

#
# Conditionalize away the unused sccsid string.
#
/^static char yysccsid\[\] = /s/.*/#ifdef UNUSED_VARS\
&\
#endif \/* UNUSED_VARS *\//

#
# Make explicit the tests of assignment results in "if" statements.
#
s|if (yyn = yydefred\[yystate\])|if (yyn = yydefred[yystate], yyn)|
s|if (yys = getenv("YYDEBUG"))|if (yys = getenv("YYDEBUG"), yys)|
