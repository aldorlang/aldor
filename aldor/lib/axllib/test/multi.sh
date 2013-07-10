#!/bin/sh
#
# This script tests compiling multiple files with a single command.

rm -f $TMPDIR/test0.as*
rm -f $TMPDIR/ovload0.as*

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
else
	ALDOR=aldor
fi

echo '== Compiling test0.as and ovload0.as'
$ALDOR -R $TMPDIR test0.as ovload0.as | grep -v "GC:"

echo '-- The files are:'
(cd $TMPDIR ; ls test0.* ovload0.*)

echo '== Cleaning up'
rm -f $TMPDIR/test0.*
rm -f $TMPDIR/ovload0.as*

echo '== Compiling null files verbosely with GC'
cd $TMPDIR
touch null1.as null2.as
$ALDOR -v -WTags+all null1.as null2.as | \
	sed -e '/version/d
		/ ti /d
		/Time /d
	        /Store/d
		/Alloc/d
		/Free /d
		/GC   /d
		/OB_/d
		/Source/d
		/Lib  /d
		/swept/d
		s/Exec: .*aldor\.exe/Exec: aldor/'

echo '== Cleaning up'
rm null[12].*
