#!/bin/sh
#
# This script tests replacement compilation of archive members.

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	DOALDOR=doaldor.sh
else
	P=
	DOALDOR=doaldor
fi
LIB=${ALDORROOT}/lib
LAXLREP=${LIB}/librepl.al

LOBREP=${LIB}/librepl.a
LOBREPLIB=${LIB}/librepl.lib
${AR} rcv ${LOBREP} /dev/null

# 'ar rcv' avoids the archive creation warning from ar.
${AR} rcv ${LAXLREP} /dev/null 2>&1 | grep -v 'ar: writing'
#if [ "$P" != "win" ]; then 
#	${AR} rcv ${LOBREP} /dev/null 2>&1 | grep -v 'ar: writing'
#fi

#
# The sed -e 's/-R *[^ ]* //g' removes the use of a -R option with an 
# absolute path so that it will remain platform independent
#
$DOALDOR arrepla repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR arreplb repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR arreplc repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR arrepla repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR arreplb repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR arreplc repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'

rm -rf ${LAXLREP} ${LOBREP} ${LOBREPLIB}
