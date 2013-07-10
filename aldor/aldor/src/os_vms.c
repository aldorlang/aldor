/****************************************************************************
 *
 * os_vms.c: Non-portable, operating system specific code for VMS.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file is meant to be included in opsys.c.
 */

#include <time.h>
#include <ctype.h>
#include <rms.h>		/* record management system (RMS$) defs */
#include <string.h>		/* C string manipulation functions */
#include <descrip.h>		/* Descriptor construction macros */
#include <lib$routines.h>	/* Declaration of RTL LIB$ routines */
#include <starlet.h>		/* Declaration of SYS$ routines */
#include <lnmdef.h>		/* Logical name (LNM) symbol definitions */
#include <ssdef.h>		/* System service (SS) symbol symbol defs. */
#include <unixio.h>		/* Emulation of UNIX RTL */
#include <unixlib.h>		/* Emulation of UNIX RTL */
#include <stdio.h>		/* C standard I/O RTL */

/*****************************************************************************
 *
 * :: osRun
 * :: osRunQuoteArg
 *
 ****************************************************************************/

#define OS_Has_Run

int
osRun(String cmd)
{
    char *p, *q, buffer[1024];
    int
	inString = false,	    /* within quoted string? */
	thisIsOption = false,	    /* this char is an option letter? */
	nextIsOption = false,	    /* this is a -, next char is an option */
	verb = true;		    /* within command verb? */

    /* If we run off the end of BUFFER the command is too long anyhow */

    for ( p = cmd, q = buffer ; *p ; p++ ) {
	if ( (verb &= *p != ' ') && *p == '"' ) continue;   /* kill " in verb */
	inString ^= *p == '"';
	thisIsOption = nextIsOption;
	nextIsOption = *p == '-' && !inString;
	if (thisIsOption) *q++ = '"';		/* quote all options letters */
	*q++ = *p;
	if (thisIsOption) { *q++ = '"'; *q++ = ' '; }
    }
    *q = *p;			    /* terminating NULL for C string */

#ifndef NDEBUG_VMS
    printf("*** cmd = %s\n***  -> %s\n", cmd, buffer);
#endif
    return !(system(buffer) & 1);   /* VMS return code: even=SUCC odd=FAIL */
}

#define OS_Has_RunQuoteArg

int
osRunQuoteArg(String word, int (*putter)(int))
{
    String	s;
    Bool	hasFunky = false;
    int	cc = 0;

    for (s = word ; *s && !hasFunky ; s++ ) {
	if ( hasFunky = isupper(*s) ) break;
	switch (*s) {
	    case '!' :
	    case '@' :
	    case '&' :
	    case '"' :
	    case '\'' :
		hasFunky = true;
	}
    }

    if (hasFunky) {
	putter('"'); cc++;
	for (s = word; *s; s++) {
	    if (*s == '"') { putter('"'); cc++; }
	    putter(*s); cc++;
	}
	putter('"'); cc++;
    }
    else
	for (s = word; *s; s++) { putter(*s); cc++; }

    return cc;
}

/*****************************************************************************
 *
 * :: osCpuTime
 *
 ****************************************************************************/

#define OS_Has_CpuTime

/* This is actually an ANSI definition. */

Millisec
osCpuTime(void)
{
	clock_t t = clock();
	return (t * 1000) / CLOCKS_PER_SEC ;
}

/*****************************************************************************
 *
 * :: osDate
 *
 ****************************************************************************/

#define OS_Has_Date

String
osDate(void)
{
	static char str[24];
	$DESCRIPTOR(date_desc, str);
	int     status;

	/* LIB$DATE_TIME returns a string of the form DD-MMM-YYYY HH:MM:SS.HH */

	status = lib$date_time(&date_desc);
	if (!status)
		lib$signal(status, NULL);	/* report unexpected errors */

	str[23] = '\0';			/* C likes NULL terminated strings */

	return str;
}

/*****************************************************************************
 *
 * :: osPutEnv
 * :: osPutEnvIsKept
 *
 ****************************************************************************/

/*
 * An ITEM_LIST_3 is a data structure required by some VMS system services,
 * including SYS$TRNLNM and SYS$CRELNM. The "item list" arguments to these
 * routines is a pointer to an array of ITEM_LIST_3 entries, terminated by
 * zero longword. Each entry contains item code indicating what the routine is
 * to do next, and a pointer to a buffer and a pointer to a length: the
 * routines only read the item list, to the buffers and lengths need to be
 * allocated by the caller.
 */

struct item_list_3 {
	UShort	buffer_length, item_code;
	String  buffer_address;
	UShort	*return_length_address;
};

#define OS_Has_GetEnv

String
osGetEnv(String envar)
{
	/*
	 * The string value returned is declared as being static, so
	 * successive calls to osGetEnv will overwrite this string. This is
	 * compatible with the semantics of the ANSI C getenv function.
	 */
	ULong		status;
	static char	equiv_buffer[LNM$C_NAMLENGTH + 1]; /* xlation of LNM */
	static UShort	equiv_length;		/* length of translated name */
	struct dsc$descriptor_s lognam;		/* logical name */
	ULong		attr;			/* LNM search attributes */
	$DESCRIPTOR(tabnam, "LNM$FILE_DEV");    /* L. name table search path */
	struct item_list_3 itmlst[] = {
		{LNM$C_NAMLENGTH, LNM$_STRING, equiv_buffer, &equiv_length},
		{0, 0, 0, 0}
	};

	lognam.dsc$a_pointer = envar;		/* string */
	lognam.dsc$w_length = strlen(envar);	/* string length (w/o null) */
	lognam.dsc$b_class = DSC$K_CLASS_S;	/* string descriptor */
	lognam.dsc$b_dtype = DSC$K_DTYPE_T;	/* ASCII string */

	attr = LNM$M_CASE_BLIND;/* ignore case in LNM searches */

	status = sys$trnlnm(
		&attr,		/* attributes */
		&tabnam,	/* LNM table(s) to search */
		&lognam,	/* logical name to translate */
		NULL,		/* acmode: ignore access mode */
		&itmlst);	/* information to return */

	if (status == SS$_NOLOGNAM) {
		equiv_length = 0;
		return NULL;
	}
	else if (!(status & 1))
		lib$signal(status, NULL);	/* report unexpected errors */

	equiv_buffer[equiv_length] = '\0';
	return equiv_buffer;
}

#define OS_Has_PutEnv

int
osPutEnv(String eqn)
{
	unsigned long int status;
	String  end_buffer;
	static unsigned long int lnm_attr;
	static String equiv_buffer;	          /* xlation of logical name */
	struct dsc$descriptor_s lognam;	          /* logical name */
	$DESCRIPTOR(tabnam, "LNM$PROCESS_TABLE"); /* logical name table */

	struct item_list_3 itmlst[] = {
		{sizeof lnm_attr, LNM$_ATTRIBUTES, (String) & lnm_attr, 0},
		{0, LNM$_STRING, 0, 0},
		{0, 0, 0, 0}
	};

	for (equiv_buffer = eqn;
		*equiv_buffer && *equiv_buffer != '=';
		equiv_buffer++);

	for (end_buffer = ++equiv_buffer; *end_buffer; end_buffer++);

	lognam.dsc$a_pointer = eqn;		/* logical name string */
	lognam.dsc$w_length = (equiv_buffer - eqn) - 1;	/* string length */
	lognam.dsc$b_class = DSC$K_CLASS_S;	/* string descriptor */
	lognam.dsc$b_dtype = DSC$K_DTYPE_T;	/* ASCII string */

	lnm_attr = 0;		/* use default attributes */
	/* LNM$M_CONCEALED|LNM$M_TERMINAL */

	itmlst[1].buffer_address = equiv_buffer;
	itmlst[1].buffer_length = end_buffer - equiv_buffer;

	/*
	 * A little explanation is required here. The system service routine
	 * $CRELNM creates a new logical name and assigns it a value and
	 * miscellaneous properties. Among the properties is the access mode,
	 * which specifies whether the logical name has USER, SUPERVISOR,
	 * EXECUTIVE, or KERNEL mode. Unfortunately, it cannot define a
	 * logical name with an access mode higher than that of the process
	 * it is running in, and for USER mode processes this means that the
	 * logical name can only be defined with USER mode. This, in turn,
	 * means that the logical name will vanish when the image exits. To
	 * cirumvent this, we may use the RTL routine LIB$SET_LOGICAL, which
	 * requests the shell under which the process is running to define
	 * the logical name for it at SUPERVISOR access mode, which survives
	 * the exit of the current image.
	 */

#ifdef SUPERVISOR_LNM
	status = lib$set_logical(
		&lognam,	/* LNM to be defined */
		NULL,		/* value (specified in itmlst) */
		&tabnam,	/* LNM table to contain new name */
		NULL,		/* attr: default attributes */
		&itmlst);	/* what to do */
#else /* SUPERVISOR_LNM */
	status = sys$crelnm(
		NULL,		/* attr: default attributes */
		&tabnam,	/* LNM table to contain new name */
		&lognam,	/* LNM to be defined */
		NULL,		/* acmode: access mode of LNM */
		&itmlst);	/* what to do */
#endif /* SUPERVISOR_LNM */

	if (!(status & 1))
		lib$signal(status, NULL);	/* report unexpected errors */

	return (status & 1) ? 0 : -1;	/* UNIX-like return code */
}

#define OS_Has_PutEnvIsKept

Bool
osPutEnvIsKept(void)
{
	return false;		/* osPutEnv doesn't care about the string
				 * anymore. */
}

/*****************************************************************************
 *
 * :: osCurDirName
 * :: osTmpDirName
 *
 * :: osObjectFileType
 * :: osExecFileType
 *
 ****************************************************************************/

String  osObjectFileType = "obj";
String  osExecFileType   = "exe";

String
osCurDirName(void)
{
	return "";
}
String
osTmpDirName(void)
{
	return "SYS$SCRATCH:";
}

/*****************************************************************************
 *
 * :: osFnameNParts
 * :: osFnameParse
 * :: osFnameParseSize
 * :: osFnameUnparse
 * :: osFnameUnparseSize
 * :: osFnameDirEqual
 * :: osFnameEqual
 *
 ****************************************************************************/

#define OS_Has_FnameIsAbsolute

/*
 *  osFnameIsAbsolute
 *
 *  We take the concept of "absolute" to mean that the file specification
 *  does not depend upon the current default directory. !! This is interpreted
 *  in a purely syntactic manner, namely a relative file specification cannot
 *  contain either a ":" (device or logical name delimiter) or a a "["
 *  (directory delimiter) which is not immediately followed by a "." (directory
 *  separator).
 *
 *  !! If the file specification is simply a logical name, it will
 *  be taken to be relative whether it is or not.
 */

local   Bool
osFnameIsAbsolute(String fname)
{
	String  p;

	for (p = fname; *p != '\0'; p++) {
		if (*p == '[')
			return (*++p != '.');
		if (*p == ':')
			return true;
	}
	return false;
}

#define OS_Has_FnameParse

Length	osFnameNParts = 3;	/* Later extend to version numbers etc. */

/*
 *  rmsFnameParse
 *
 *  This routine uses RMS calls ($PARSE, $OPEN) to parse a VMS filespec
 *  and return an array of 3 strings (PARTS) which contain the node+device+
 *  directory, name, and type+version respectively. The storage for these
 *  strings will reside in the memory pointed to by BUFFER. If DID is zero
 *  then RMS will only be asked to do a syntax check and no actual I/O
 *  (The NAM$B_NOP field is set to NAM$M_SYNCHK). If DID is non-zero then a
 *  $PARSE call will be made which will check that the directory in which the
 *  file could live exists, and its fileid (an array of three words pointed)
 *  will be returned via the DID argument. If FID is non-zero then an $OPEN
 *  call will be made to verify the existence of the specified file and its
 *  fileid will be returned via the FID argument.
 */

static int
rmsFnameParse(String filespec, String default_name,
	String * parts, String buffer,
	UShort * did, UShort * fid)
{
	unsigned long int status;
	char    esa[NAM$C_MAXRSS];
	String  ptr;
	int     i;

	struct FAB fab;
	struct NAM nam;

	if (!default_name)
		default_name = "";

#ifdef DEBUGRMS
	printf(">> rmsFnameParse:\n");
	printf("<   filespec = \"%s\"\n", filespec);
	printf("<   default_name = \"%s\"\n", default_name);
	printf(">   check_dir = %s\n", did ? "true" : "false");
	printf(">   check_file = %s\n", fid ? "true" : "false");
#endif /* DEBUGRMS */

	fab = cc$rms_fab;
	nam = cc$rms_nam;

	fab.fab$l_dna = default_name;
	fab.fab$b_dns = strlen(default_name);

	fab.fab$l_fna = filespec;
	fab.fab$b_fns = strlen(filespec);

	fab.fab$l_nam = &nam;

	nam.nam$l_esa = esa;
	nam.nam$b_ess = sizeof esa;

	if (!did)
		nam.nam$b_nop = NAM$M_SYNCHK;

	if (fid)
		status = sys$open(&fab);
	else
		status = sys$parse(&fab);

	if (!(status & 1)) {	            /* if unsuccessful... */
		if (status != RMS$_FNF      /* and not "file not found" */
		&& status != RMS$_DNF	    /* and not "directory not found" */
		&& status != RMS$_DEV	    /* and not "error in device name" */
		&& status != RMS$_SYN)	    /* and not "file spec syntax err" */
			lib$signal(status, fab.fab$l_stv);
					    /* report unexpected errors */
		return 1;	            /* ... and fail */
	}

#ifdef DEBUGRMS
	esa[nam.nam$b_esl] = '\0';
	printf("> Expanded filespec = \"%s\"\n", nam.nam$l_esa);
#endif	/* DEBUGRMS */

	/*
	 * We must copy the expanded file name from the expanded string area
	 * (ESA) into our own buffer because C requires NUL terminated
	 * strings and we need somewhere to put the extra NULs. If we wanted
	 * to return the data in the ESA area directly we would have to
	 * allocate it as a static variable.
	 *
	 * osFname's model does not include leading or trailing punctuation as
	 * part of the file parts, while VMS's model does, so we strip off
	 * punctuation here.
	 *
	 * !! osFname's model does not include version numbers.
	 * We therefore ignore them here. This is WRONG if the user wants to
	 * compile version ;-3 of a file, for example.
	 */

	if (parts) {		/* skip if parts argument is zero */
		*parts++ = ptr = buffer;
		if (*default_name != '\0') {
			strncpy(ptr, nam.nam$l_node, nam.nam$b_node);
			strncpy(ptr += nam.nam$b_node, nam.nam$l_dev, nam.nam$b_dev);
			strncpy(ptr += nam.nam$b_dev, nam.nam$l_dir, nam.nam$b_dir);
			*(ptr += nam.nam$b_dir) = '\0';
		}
		else *ptr++ = '\0';

		strncpy(*parts++ = ++ptr, nam.nam$l_name, nam.nam$b_name);
		*(ptr += nam.nam$b_name) = '\0';

		strncpy(*parts++ = ++ptr, nam.nam$l_type+1, nam.nam$b_type-1);
		*(ptr += nam.nam$b_type - 1) = '\0';

	/*	strncpy(ptr += nam.nam$b_type, nam.nam$l_ver, nam.nam$b_ver);*/
	/*	*(ptr += nam.nam$b_ver) = '\0';				     */
	}

	/* copy the FID and DID values as appropriate */
	if (did)
		for (i = 0; i < 3; i++)
			did[i] = nam.nam$w_did[i];
	if (fid)
		for (i = 0; i < 3; i++)
			fid[i] = nam.nam$w_fid[i];

	/* close the file in the case where we had to open it */
	if (fid) {
		status = sys$close(&fab);
		if (!(status & 1))
			lib$signal(status, fab.fab$l_stv);
		/* report unexpected errors */
	}
	return !(status & 1);	/* UNIX-style error code */
}

/*
 * For VMS we shall assume that a "relative to" string is the same as
 * a "default filespec", so
 *
 * osFnameParse(*, *, "NAME.TYPE;2", "DEV:[DIR.SUBDIR]")
 * --> DEV:[DIR.SUBDIR]NAME.TYPE;2
 *
 * osFnameParse(*, *, "NAME.TYPE;2", "")
 * --> NAME.TYPE;2 (relative to default directory)
 *
 * osFnameParse(*, *, "[EXPLICIT_DIR]NAME.TYPE;2", "DEV:[DIR.SUBDIR]")
 * --> DEV:[EXPLICIT_DIR]NAME.TYPE;2
 */
void
osFnameParse(String *partv, String buffer, String fname, String relativeTo)
{
	char    defdir[NAM$C_MAXRSS + 1];

	rmsFnameParse(fname, relativeTo, partv, buffer, NULL, NULL);

	/*
	 * Clear directory specification if it is the current default directory
	 */
	if (osFnameDirEqual(getcwd(defdir, NAM$C_MAXRSS), *partv))
		**partv = '\0';

	return;
}

Length
osFnameParseSize(String fname, String relativeTo)
{
	Length	sz = 0;

	sz += strlen(fname);
	sz += relativeTo ? strlen(relativeTo) + 3 : 0;
	sz += osFnameNParts;

	return sz;
}

String
osFnameUnparse(String buffer, String *partv, Bool full)
{
	String dir = partv[0], name = partv[1], type = partv[2];
	if (!dir)  dir  = "";
	if (!name) name = "";
	if (!type) type = "";

	*buffer = '\0';

	/* Only give the directory if it is not the current one. */
	if (*dir && (full || strcmp(dir, osCurDirName())))
	    strcat(buffer, dir);
	strcat(buffer, name);
	strcat(buffer, ".");	/* add punctuation */
	strcat(buffer, type);

	return buffer;
}

/*
 * osFname parts exclude their punctuation, and remember to add 1
 * for the trailing NULL C string delimiter.
 */
Length
osFnameUnparseSize(String *partv, Bool full)
{
	String dir = partv[0], name = partv[1], type = partv[2];
	if (!dir)  dir  = "";
	if (!name) name = "";
	if (!type) type = "";

	return ((*dir && (full||strcmp(dir, osCurDirName())))
	    ? strlen(dir) : 0) + strlen(name) + strlen(type) + 2 + 1;
}

Length
osSubdirLength(String relativeTo, String subdir)
{
#ifdef DEBUGRMS
	printf(">> osSubdirLength(\"%s\",\"%s\") = %d\n",
		relativeTo, subdir,
		strlen(subdir) + (!*relativeTo ? 2 : 1 + strlen(relativeTo)));
#endif /* DEBUGRMS */

	return strlen(subdir) + (!*relativeTo ? 2 : 1 + strlen(relativeTo));
}

/*
 * !! Warning: There is potential for future problems here.
 *
 * This is not a very natural VMS concept, so we just glue the SUBDIR
 * name onto the end of the directory specification, e.g., [A.B] + C
 * -> [A.B.C]. If this results in a directory specification
 * containing more than eight levels then the filename will be
 * invalid. Caveat emptor (it is too hard to check for all perverse
 * combinations of syntax elements like "...", "--", rooted
 * directories ([A.]), relative directories ([.A]), and so forth.
 */
void
osSubdir(String buf, String relativeTo, String subdir)
{
	String  p;
	char    c;

	p = buf;
	for (c = *relativeTo; c && c != '[' && c != '<'; c = *++relativeTo)
		*p++ = c;
	*p = '[';
	if (*relativeTo)
		relativeTo++;
	while (*relativeTo)
		*++p = *relativeTo++;
	if (*p != '[')
		*p = '.';
	while (*subdir)
		*++p = *subdir++;
	*++p = ']';
	*++p = '\0';

#ifdef DEBUGRMS
	printf(">> osSubdir(\"%s\",\"%s\",\"%s\")\n", buf, relativeTo, subdir);
#endif /* DEBUGRMS */

	return;
}

/*
 * In VMS a directory specification is of the form [A.B.C], which is
 * different from a file specification such as [A.B]C.DIR which is
 * the name of the directory file itself. We shall compare the
 * directory portion of a file name and ignore the file name itself
 * (to be precise, we compare the node, device, and directory fields,
 * and ignore the name, type, and version fields). In the case were
 * the directory exists we check its internal directory identifier
 * (DID); if it does not exist we resort to comparing the node,
 * device, and directory fields for string equality. In the former
 * case we can guarantee not to be fooled by aliases: in the latter
 * case the question is moot.
 */
#define OS_Has_FnameDirEqual

Bool
osFnameDirEqual(String d1, String d2)
{
	Bool   equal;
	UShort did1[3], did2[3];
	char   *parts1[3], *parts2[3],
		buffer1[NAM$C_MAXRSS], buffer2[NAM$C_MAXRSS];

	/* First check for syntactic equality */

	rmsFnameParse(d1, "", parts1, buffer1, NULL, NULL);
	rmsFnameParse(d2, "", parts2, buffer2, NULL, NULL);
	equal = !strcmp(parts1[0], parts2[0])
		&& !strcmp(parts1[1], parts2[1])
		&& !strcmp(parts1[2], parts2[2]);

	/*
	*  If not blatantly equal investigate further (at the cost of
	*  doing some actual $QIOs)
	*/

	if (!equal)
	    if (!rmsFnameParse(d1, "", NULL, NULL, did1, NULL)
		    && !rmsFnameParse(d2, "", NULL, NULL, did2, NULL)) {
		    equal = did1[0] == did2[0]
			    && did1[1] == did2[1]
			    && did1[2] == did2[2];
	    }

#ifdef DEBUGRMS
	printf(">> osFnameDirEqual(\"%s\",\"%s\") = %s\n",
		d1, d2, equal ? "true" : "false");
#endif /* DEBUGRMS */

	return equal;
}

#define OS_Has_FnameEqual

Bool
osFnameEqual(String f1, String f2)
{
	Bool   equal;
	UShort fid1[3], fid2[3];
	char   *parts1[3], *parts2[3],
		buffer1[NAM$C_MAXRSS], buffer2[NAM$C_MAXRSS];

	/* First check for syntactic equality */

	rmsFnameParse(f1, "", parts1, buffer1, NULL, NULL);
	rmsFnameParse(f2, "", parts2, buffer2, NULL, NULL);
	equal = !strcmp(parts1[0], parts2[0])
		&& !strcmp(parts1[1], parts2[1])
		&& !strcmp(parts1[2], parts2[2]);

	/*
	*  If not blatantly equal investigate further (at the cost of
	*  doing some actual $QIOs)
	*/

	if (!equal)
	    if (!rmsFnameParse(f1, "", NULL, NULL, NULL, fid1)
		    && !rmsFnameParse(f2, "", NULL, NULL, NULL, fid2)) {
		    equal = fid1[0] == fid2[0]
			    && fid1[1] == fid2[1]
			    && fid1[2] == fid2[2];
	    }

	/*
	*  Final attempt: perhaps the files don't exist yet, their
	*  directories exist but are accessed by different names
	*  and the files' names and types are the same...
	*/

	if (!equal)
	    if (osFnameDirEqual(f1, f2))
		equal = !strcmp(parts1[1], parts2[1])
			&& !strcmp(parts1[2], parts2[2]);

#ifdef DEBUGRMS
	printf(">> osFnameEqual(\"%s\",\"%s\") = %s\n",
		f1, f2, equal ? "true" : "false");
#endif /* DEBUGRMS */

	return equal;
}

/*****************************************************************************
 *
 * :: Directory path manipulation.
 *
 ****************************************************************************/

#define FPATHSEP	 ','	/* !! What does this mean for VMS? */

/*****************************************************************************
 *
 * :: osIsInteractive
 * :: osDirIsThere
 * :: osDirSwap
 *
 ****************************************************************************/

#define OS_Has_IsInteractive

Bool
osIsInteractive(FILE *f)
{
	return isatty(fileno(f));	/* UNIXIO.H defines isatty */
}

#define OS_Has_DirIsThere
/*
 * This routine is supposed to deduce whether the file specified in the NAME
 * argument is a directory or not. For VMS this is treated as a purely syntactic
 * matter: the syntax for a directory, NODE::DEVICE:[D1.D2.D3], is distinct from
 * that for a file, NODE::DEVICE:[D1.D2.D3]NAME.TYPE;VERSION, i.e., a file must
 * have a name and/or a type. Minimally, a file can have an empty name and a
 * type of just ".".
 *
 * A separate question is whether a file is itself a directory, as directories
 * in VMS are files. They are distinguished from "ordinary" files by the
 * following characteristics:
 *
 *   The FCH$M_DIRECTORY bit in the file characteristics field ATR$C_UCHAR
 *     (see the I/O User's Reference Manual, section 1.3.5.1). This bit
 *     could be inspected by issuing a sense-mode SYS$QIO system call, but
 *     it doesn't really seem worth the effort.
 *
 *   The file type must be .DIR and the version number must be ;1. If these
 *     criteria are not met then VMS will not treat the file as a directory,
 *     even if it has the internal structure of one.
 *
 *   A directory must have a non-empty name, i.e., .DIR is not a valid directory
 *     name. If it were then [...] would be ambiguous!
 */
Bool
osDirIsThere(String name)
{
	String partv[3 /* osFnameNParts */];
	char buffer[NAM$C_MAXRSS + 1];
	Bool isadir;

	/* !! Parse the filespec NAME with a very unlikely default */

	osFnameParse(partv, buffer, name, "999979999.994999;6468");

	isadir = strcmp(partv[1], "999979999") == 0;
	isadir &= strcmp(partv[2], "994999") == 0;
	/* isadir &= strcmp(partv[3], "6468"); */
	
#ifdef DEBUGRMS
	printf(">> osDirIsThere(%s) -> %s\n", name, isadir ? "true" : "false" );
#endif /* DEBUGRMS */

	return isadir;
}

#define OS_Has_DirSwap

int
osDirSwap(String dest, String orgbuf, Length orgsiz)
{
	String  s;
	int     rc;

	if (orgbuf) {
		s = getcwd(orgbuf, orgsiz);	/* UNIXLIB.H defines getcwd */
		if (!s) return -1;
	}

	rc = chdir(dest);
	if (rc == -1) return -1;

	return 0;
}

/*****************************************************************************
 *
 * :: osMemMap
 *
 ****************************************************************************/

