#!/bin/sh
#
# This script tests replacement compilation of archive members.

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	DOALDOR=doaldor.sh
else
	DOALDOR=doaldor
fi

LIB=${ALDORROOT}/lib
LAXLREP=${LIB}/librepl.al
LOBREP=${LIB}/librepl.a
LOBREPLIB=${LIB}/librepl.lib

# 'ar rcv' avoids the archive creation warning from ar.
${AR} rcv ${LAXLREP} /dev/null 2>&1 | grep -v 'ar: writing'
${AR} rcv ${LOBREP} /dev/null 2>&1 | grep -v 'ar: writing'

#
# The perl -p -e 's/-R *[^ ]* //g' removes the use of a -R option with an 
# absolute path so that it will remain platform independent
#
# The perl -p -e 's/-M no-ALDOR_W_OverRideLibraryFile //' eliminates the 
# -M no-ALDOR_W_OverRideLibraryFile so that it will match the installed output
#
$DOALDOR arrepl1a repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR arrepl1b repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR arrepl1c repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR arrepl1a repl -Mno-warnings -D AddExport 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
#$DOALDOR arrepl1b repl -Mno-warnings
$DOALDOR arrepl1c repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'

rm -rf ${LAXLREP} ${LOBREP} ${LOBREPLIB}
