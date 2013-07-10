#!/bin/sh
#----------------------------------------------------------------------
# @(#)maintain.sh	2.12	10/31/89 15:34:25
#
# maintain.sh -- 08/16/89 Florian Bundschuh
#			  (florian@matthew.watson.ibm.com)
#             -- 04/16/91 Work-around for sact bug on RS/6000 (##!!1)
#             -- 11/08/93 Modified to work on ALDOR directories.
#
# This shell script expects a command name,
#	add, asis, erase, diff, edit, get, hide, history, lock, mkdir,
#	mv, mvdir, purge, query, rep, unlock,
# and a filename; this filename must be rooted wrt "$ALDORROOT" and it is
# in every case the name of a plain file or directory, not that of an
# SCCS s-file.	This shell script is normally called through "maintain".
#
# This command file is intended to be used for maintaining the
# code, i.e., to provide a locking/unlocking mechanism,
# to keep older versions of files, and to log changes.	"maintain"
# is basically a front end to SCCS, but uses the syntax of the
# VM MAINTAIN command and allows to handle non-ascii files.  For
# these, only one regress version is held, and the regress file is only
# overwritten if it is older than 1 day. - If the user writes
#
#		maintain cmd dirname/basename
#
# then the plain file itself will be in $CUR/dirname, whereas the
# SCCS files are kept in a different directory, the regress directory.
#
# Although SCCS prepends strings like "s." and "p." to a filename,
# filenames with up to 14 characters are handled reasonably unless
# two different files have names that don't differ in their first
# 12 characters.
#
# The environment variable VI should be set to your favourite
# editor in order to "macomp edit" a file.  VI is by default
# /usr/bin/vi.
#
# Notes:  (1) VI has been used instead of EDITOR because EDITOR is used
# by /usr/bin/man.  (2) "get" works like MAINTAIN GET - it locks a
# file and copies it into your current directory without expanding the
# SCCS keywords; the file is then writable by you.  However, the
# SCCS-get would expand the keywords while copying the file and would
# not lock it; the file would be read-only.  (3) There is one known
# racing problem:  If you "add" or "rep" a source file and someone
# else "edit"s and "rep"s it extremely fast, it could happen that
# your file overwrites his or her newer version in the directory
# where the plain files reside.	 The SCCS files, however, are
# up-to-date in any case.  (4) Because of our notion that "maintain"
# should always keep the maintained file also as plain file, the user
# must be very careful if he uses the SCCS feature of introducing
# different branches of a file.	 (5) Some "maintain" commands hand
# options, which are introduced by '-' and follow the filename, through
# to the underlying SCCS commands.  Especially useful are "-r2.3" and
# "-y'No comment!'" for add, purge, and rep to give a version (SID)
# number and a comment.	 "rep" asks for a comment if the user doesn't
# supply one with "-y".
#----------------------------------------------------------------------
trap 'rm -f /tmp/$$ /tmp/${Base-$$} /tmp/get[abc]$$; exit 1' 1 2 3 15
 
Usage=\
"Usage:\tmaintain (add|edit|erase|get|hide|history|lock|rep|unlock) file [opts]
 -or-\tmaintain diff file [-rSID1 [-rSID2]]
 -or-\tmaintain (asis|mkdir|purge|query) subdir [opts]
 -or-\tmaintain mvdir subdir1 subdir2
 -or-\tmaintain mv file1 file2"
BAK=$ALDORSERV/../../regress
BEL=
CUR=$ALDORSERV/../..
PATH=/bin:/usr/bin
EXECDIR=$ALDORSERV/mabin
CHMOD="chmod u=rw,go=r"
umask 022
 
if [ $# -lt 2 -o \( "$1" = "mv" -o "$1" = "mvdir" \) -a $# -ne 3 ] ; then
	echo "$Usage" 1>&2
	exit 1
fi
 
# Get editor for the "edit" subcommand.	 Default is /usr/bin/vi.
if [ ! "$VI" ] ; then
	VI=/usr/bin/vi
fi
 
# Which command?
Cmd=$1
shift
 
# Which subdirectory is to be maintained?
case "$Cmd" in
add | diff | edit | erase | get | hide | history  | lock | mv | rep | unlock)
	Dir=`dirname $1`
	Base=`basename $1 2>/dev/null`
	if [ ! "$Base" ] ; then
		echo "Basename part of '$1' empty." 1>&2
		echo "$Usage" 1>&2
		exit 1
	fi
	;;
asis | mkdir | mvdir | purge | query)
	Dir=$1
	;;
*)	echo "Command '$Cmd' unknown." 1>&2
	echo "$Usage" 1>&2
	exit 1
esac
if [ "$Dir" = "." ] ; then
	echo "Dirname part of '$1' empty." 1>&2
echon "Do you really want to maintain '$ALDORSERV' itself? (n/y) " 1>&2
	read response
	case "$response" in
	[yY]*)	:
		;;
	*)	echo "$Usage" 1>&2
		exit 1
	esac
fi
if [ -d $CUR/$Dir -o "$Cmd" = "mkdir" ] ; then
	PLAINDIR=$CUR/$Dir
	SCCSDIR=$BAK/$Dir
else
	echo "Subdirectory not found: $Dir." 1>&2
	echo "$Usage" 1>&2
	exit 1
fi
if [ ! -d "$SCCSDIR" ] ; then
	case "$Cmd" in
	add|diff|edit|get|hide|history|lock|purge|query|rep|unlock)
		echo "Regress directory not yet created." 1>&2
		echo "Consider issuing: maintain mkdir $Dir" 1>&2
		exit 1
	esac
fi
shift
 
# "Source" or "object" file?
case "$Base" in
*.dvi | *.ao | *.ps | *.tgz)
	type="object"
	;;
*)
	type="source"
esac
 
# Subroutine for "add"
Add() {
	if [ "$type" = "source" ] ; then
		if [ X"$@" = X"" ] ; then
			admin -i$Base $SCCSDIR/s.$Base
		else
			admin -i$Base "$@" $SCCSDIR/s.$Base
		fi
		if [ $? -ne 0 ] ; then
echo $BEL "A problem occurred while installing the regress file.
Plain file '$Dir/$Base' not created." 1>&2
			return 1
		fi
		if [ `pwd` = "$PLAINDIR" ] ; then
			$CHMOD $Base
			return
		fi
		if [ -f $PLAINDIR/$Base ] ; then
echo $BEL "Warning:  There was already a plain file - not overwritten." 1>&2
			return 1
		fi
		# The "cat" hack is necessary to keep "get" quiet.
		get -s -p $SCCSDIR/s.$Base | cat >$PLAINDIR/$Base
		if [ $? -ne 0 ] ; then
			return 1
		fi
		$EXECDIR/suru rm -f $Base
	else # "$type" = "object"
		(cd /tmp
		 echo "$Base" >$$
		 admin -i$$ $SCCSDIR/s.$Base
		 status=$?
		 rm -f $$
		 exit $status)
		if [ $? -ne 0 ] ; then
			return 1
		fi
		cp $Base $SCCSDIR/$Base
		if [ $? -ne 0 ] ; then
echo "Couldn't write regress file, 'add' failed, now cleaning up ..." 1>&2
			rm -f $SCCSDIR/s.$Base
			return 1
		fi
		$CHMOD $SCCSDIR/$Base
		if [ `pwd` = "$PLAINDIR" ] ; then
			$CHMOD $Base
			return
		fi
		cp $Base $PLAINDIR/$Base
		if [ $? -ne 0 ] ; then
echo "Couldn't write plain file, 'add' failed, now cleaning up ..." 1>&2
			rm -f $SCCSDIR/s.$Base $SCCSDIR/$Base
			return 1
		fi
		$CHMOD $PLAINDIR/$Base
		$EXECDIR/suru rm -f $Base
	fi
}
 
# Subroutine for "asis"
AsIs() {
	if [ ! -d $SCCSDIR ] ; then
		mkdir $SCCSDIR
	fi
	cd $PLAINDIR
	for i in * ; do
		if [ ! -f $i ] ; then
			continue
		fi
		if [ -f $SCCSDIR/s.$i ] ; then
echo "Warning:	There is already a regress file for '$i'." 1>&2
			continue
		fi
		echo "Now adding '$i'." 1>&2
		Base=$i
		Add
		if [ $? -ne 0 ] ; then
echon "A problem occured while adding '$i'.  Continue? (y/n) " 1>&2
			read response
			case "$response" in
			[nN]*)	exit 1
				;;
			*)	:
			esac
		fi
	done
}
 
# Subroutine for "diff"
# This is basically /usr/bin/sccsdiff with some additional gadgets.
# Error status (like diff):
#	0 - ok, and no differences
#	1 - ok, but some differences
#	2 - some error
Diff() {
	if [ "$type" = "object" ] ; then
		echo "'diff' only useful for source files." 1>&2
		return
	fi
	sid1= sid2= num= pipe=
	for i in $@ ; do
		case $i in
		-[rsp]*)
			case $i in
			-r*)	if [ ! "$sid1" ] ; then
					sid1=$i
				elif [ ! "$sid2" ] ; then
					sid2=$i
				fi
				;;
			-s*)	num=`expr "$i" : '-s\(.*\)'`
				;;
			-p*)	pipe=yes
			esac
			;;
		*)	echo "$0 $1: Unknown argument '$i'." 1>&2
			echo "$Usage" 1>&2
			exit 2
		esac
	done
	if get -s -k -p $sid1 $SCCSDIR/s.$Base >/tmp/geta$$
	then
		if [ -z "$sid2" ] ; then
			cp $Base /tmp/getb$$
		else
			get -s -k -p $sid2 $SCCSDIR/s.$Base >/tmp/getb$$
		fi
		if [ $? -eq 0 ] ; then
			bdiff /tmp/geta$$ /tmp/getb$$ $num >/tmp/getc$$
		fi
	fi
	status=1
	if [ ! -s /tmp/getc$$ ] ; then
		if [ -f /tmp/getc$$ ] ; then
			echo "$Base: No differences" >/tmp/getc$$
			status=0
		else
			exit 2
		fi
	fi
	if [ "$pipe" ] ; then
		pr -h "$i: $sid1 vs. $sid2" /tmp/getc$$
	else
		cat /tmp/getc$$
	fi
	rm -f /tmp/get[abc]$$
	exit $status
}
 
# Subroutine for "edit"
Edit() {
	if [ "$type" = "source" ] ; then
		if [ X"$@" = X"" ] ; then
			get -e $SCCSDIR/s.$Base
		else
			get -e "$@" $SCCSDIR/s.$Base
		fi
		if [ $? -eq 0 ] ; then
			$EXECDIR/suru $VI $Base
		fi
	else
		if [ -w $Base ] ; then
			echo "ERROR: Writable '$Base' exists." 1>&2
			exit 1
		fi
		get -e -p $SCCSDIR/s.$Base >/dev/null
		if [ $? -eq 0 ] ; then
			$EXECDIR/suru cp $PLAINDIR/$Base $Base
			exec $EXECDIR/suru $VI $Base
		fi
	fi
}
 
# Subroutine for "erase"
Erase() {
	if [ -f $PLAINDIR/$Base ] ; then
		echo "'$Base' is not yet hidden: not erased." 1>&2
		exit 1
	fi
	rm -f $SCCSDIR/$Base $SCCSDIR/s.$Base $SCCSDIR/p.$Base
}
 
# Subroutine for "get"
Get() {
	if [ "$type" = "source" ] ; then
		if [ X"$@" = X"" ] ; then
			get -e $SCCSDIR/s.$Base
		else
			get -e "$@" $SCCSDIR/s.$Base
		fi
	else
		if [ -w $Base ] ; then
			echo "ERROR: Writable '$Base' exists." 1>&2
			return 1
		fi
		get -e -p $SCCSDIR/s.$Base >/dev/null
		if [ $? -eq 0 ] ; then
			$EXECDIR/suru cp $PLAINDIR/$Base $Base
		fi
	fi
}
 
# Subroutine for "hide"
Hide() {
	if [ ! -f $PLAINDIR/$Base ] ; then
		echo "The file '$PLAINDIR/$Base' does not exist." 1>&2
		exit 1
	fi
	fgrep `$EXECDIR/realuser` $SCCSDIR/p.$Base >/dev/null 2>&1
	case $? in
	2)	echo "'$Base' is not locked." 1>&2
		exit 1
		;;
	1)	echo "'$Base' is not locked by you:" 1>&2
		cat $SCCSDIR/p.$Base
		exit 1
		;;
	0)	:
	esac
	rm -f $PLAINDIR/$Base
}
 
# Subroutine for "history"
History() {
	if [ "$type" = "object" ] ; then
		echo "'hist' only useful for source files." 1>&2
		return
	fi
	if [ ! -f $SCCSDIR/s.$Base ] ; then
		echo "There is no regress file for '$Dir/$Base'." 1>&2
		exit 1
	fi
	prs $SCCSDIR/s.$Base | sed -e "/^MRs:$/d" -e "/^COMMENTS:$/d" | cat
}
 
# Subroutine for "mkdir"
Mkdir() {
	if [ -d $PLAINDIR ] ; then
echo "Plain directory exists already; now creating regress directory." 1>&2
	else
		mkdir $PLAINDIR
	fi
	if [ -d $SCCSDIR ] ; then
		echo "Regress directory exists already." 1>&2
	else
		mkdir $SCCSDIR
	fi
}
 
# Subroutine for "mv"
Mv() {
	Dir2=`dirname $1`
	Base2=`basename $1 2>/dev/null`
	if [ ! "$Base" ] ; then
		echo "Basename part of second filename empty." 1>&2
		echo "$Usage" 1>&2
		exit 1
	fi
	if [ "$Dir2" = "." ] ; then
		Dir2=$Dir
	fi
	if [ ! -d $CUR/$Dir2 ] ; then
		echo "Target plain directory does not exist: giving up."  1>&2
		exit 1
	fi
	if [ ! -d $BAK/$Dir2 ] ; then
		echo "Target regress directory does not exist: giving up."  1>&2
		exit 1
	fi
	if [ -f $CUR/$Dir2/$Base2 ] ; then
		echo "Target file exists already: giving up." 1>&2
		exit 1
	fi
	mv $PLAINDIR/$Base $CUR/$Dir2/$Base2
	if [ -f $SCCSDIR/$Base ] ; then
		mv $SCCSDIR/$Base $CUR/$Dir2/$Base2
	fi
	# Now make sure that "base" has at most 12 characters.
	base=`expr "$Base" : "\(............\).*"`
	base=${base:-$Base}
	if [ ! -f $SCCSDIR/?.$base ] ; then
		# no regress files
		return
	fi
	for i in $SCCSDIR/?.$base ; do
		j=`basename $i`
		mv $i $BAK/$Dir2/`expr "$j" : "\(.\).*"`.$Base2
	done
}
 
# Subroutine for "mvdir"
Mvdir() {
	if [ -d $CUR/$1 ] ; then
		echo "New plain directory exists already: giving up." 1>&2
		exit 1
	fi
	if [ -d $BAK/$1 ] ; then
		echo "New regress directory exists already: giving up." 1>&2
		exit 1
	fi
	/etc/mvdir $PLAINDIR $CUR/$1
	/etc/mvdir $SCCSDIR  $BAK/$1
}
 
# Subroutine for "purge"
Purge() {
	if [ "$type" != "source" ] ; then
		exit
	fi
	cd $PLAINDIR
	for i in * ; do
		if [ -f $i -a ! -f $SCCSDIR/s.$i ] ; then
			echo "There is no regress file for '$Dir/$i'." 1>&2
echon "Keep (k) or remove (r) plain file and continue, or abort 'purge' (a)? " 1>&2
			read response
			case "$response" in
			[aA]*)	exit 1
				;;
			[rR]*)	rm -f $i
				;;
			*)	:	# go on
			esac
		fi
		if [ -f $SCCSDIR/p.$i ] ; then
			whom=`cut -f3 -d" " $SCCSDIR/p.$i`
			echo "Warning: '$Dir/$i' was locked by '$whom'." 1>&2
		fi
	done
	cd /tmp
	for i in $PLAINDIR/* ; do
		Base=`basename $i`
		if [ ! -f $i -o ! -f $SCCSDIR/s.$Base ] ; then
			continue
		fi
		rm -f $Base
		# get latest version, w. unexpanded keywords.
		get -s -k $SCCSDIR/s.$Base
		if [ $? -ne 0 ] ; then
			echo "'get $Dir/$Base' failed; 'purge' aborted." 1>&2
			exit 1
		fi
		# Now make sure that "base" has at most 12 characters.
		base=`expr "$Base" : "\(............\).*"`
		base=${base:-$Base}
		rm -f $SCCSDIR/?.$base
		if [ X"$@" = X"" ] ; then
			admin -i$Base $SCCSDIR/s.$Base
		else
			admin -i$Base "$@" $SCCSDIR/s.$Base
		fi
	done
}
 
# Subroutine for "query"
Query() {
	##!!1 The call to sact in the following line doesn't work on RS/6000
#	sact $SCCSDIR 2>/dev/null >/tmp/$$
#	ed - /tmp/$$ << EOF
#	1m$
#	g/^/.,/^$/j
#	g/^/s/^.*\/s\.//
#	v/ /d
#	w
#	q
#EOF
#	if [ ! -s /tmp/$$ ] ; then
#		echo "No files locked."
#	else
#		cat /tmp/$$
#	fi
#	rm -f /tmp/$$

	##!! Replaced by:
	currentdir=`pwd`
	cd $SCCSDIR

	anylocked=no
	for f in `(ls p.* | sed -e 's/p\.//') 2>/dev/null `
	do
		if [ -f p.$f ] ; then
			echo "$f: `cat p.$f`"
			anylocked=yes
		fi
	done

	if [ "$anylocked" = "no" ] ; then
		echo "No files locked"
	fi

	cd $currentdir
}
 
# Subroutine for "rep"
Rep() {
	if [ "$type" = "source" ] ; then
		fgrep `$EXECDIR/realuser` $SCCSDIR/p.$Base >/dev/null 2>&1
		case $? in
		2)	echo "'$Base' is not locked." 1>&2
			exit 1
			;;
		1)	echo "'$Base' is not locked by you:" 1>&2
			cat $SCCSDIR/p.$Base
			exit 1
			;;
		0)	:
		esac
		if [ X"$@" = X"" ] ; then
			delta $SCCSDIR/s.$Base
		else
			delta "$@" $SCCSDIR/s.$Base
		fi
		if [ $? -eq 0 ] ; then
			# The "cat" hack is necessary to keep "get" quiet.
			get -s -p $SCCSDIR/s.$Base | cat >$PLAINDIR/$Base
#                       touch $CUR/src/timestamp
		else
echo $BEL "A problem occurred while updating the regress file.
Plain file '$Dir/$Base' not changed." 1>&2
		fi
	else
		fgrep `$EXECDIR/realuser` $SCCSDIR/p.$Base >/dev/null 2>&1
		case $? in
		2)	echo "'$Base' is not locked." 1>&2
			exit 1
			;;
		1)	echo "'$Base' is not locked by you:" 1>&2
			cat $SCCSDIR/p.$Base
			exit 1
			;;
		0)	:
		esac
		$EXECDIR/older24h $SCCSDIR/$Base
		case $? in
		0 | 2)	mv -f $PLAINDIR/$Base $SCCSDIR/$Base
			if [ $? -ne 0 ] ; then
echo "Couldn't write regress file, 'rep' failed." 1>&2
				exit 1
			fi
			$CHMOD $SCCSDIR/$Base
			;;
		1)
echo "Current regress version kept because newer than 1 day;" 1>&2
echo "now updating plain file." 1>&2
		esac
		cp $Base $PLAINDIR/$Base
		if [ $? -ne 0 ] ; then
echo "Couldn't write plain file, 'rep' failed, cleaning up ..." 1>&2
			if [ ! -s $PLAINDIR/$Base ] ; then
				cp $SCCSDIR/$Base $PLAINDIR/$Base
			fi
			exit 1
		fi
		$CHMOD $PLAINDIR/$Base
		$EXECDIR/suru rm -f $Base
		unget -n $SCCSDIR/s.$Base
	fi
}
 
# Parse and execute subcommand.
# Principle:  If the operation locks the file, do this first.
#	      If the operation unlocks the file, do this last.
# The names of the (primary) subdirectory and file to be maintained
# is transferred through global variables to save "shift" operations.
# The additional parameters given to Add, Edit, Get, and Rep are
# options handed over to the underlying SCCS functions.
case "$Cmd" in
add)	Add "$@"
	;;
asis)	AsIs
	;;
diff)	Diff "$@"
	;;
edit)	Edit "$@"
	;;
erase)	Erase
	;;
get)	Get "$@"
	;;
hide)	Hide
	;;
history) History
	;;
mkdir)	Mkdir
	;;
mv)	Mv $1
	;;
mvdir)	Mvdir $1
	;;
lock)	get -e -p $SCCSDIR/s.$Base >/dev/null
	;;
purge)	Purge "$@"
	;;
query)	Query
	;;
rep)	Rep "$@"
	;;
unlock) unget -n $SCCSDIR/s.$Base
	;;
*)	echo "$Usage" 1>&2
	exit 1
esac
