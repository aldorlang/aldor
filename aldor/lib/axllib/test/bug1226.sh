#!/bin/sh
#
# From bug submitted by Manuel Bronstein.
# 
# ------------------------------ sflo.as ------------------------------
# --
# -- Seems like the SFlo$Machine <--> float equivalence is broken
# -- This problem happens only on linux, works ok under OSF1
# --
# -- % unicl -c sfloc.c
# -- % aldor -fx -laxllib sflo.as sfloc.o
# -- % ./sflo
# -- sf = 3.1400001
# -- sflo = 36893488147419103232.000000
# --

cd $TMPDIR

if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	OBJ=obj
else
	OBJ=o
fi

cat << EOF > bug1226.as
#include "axllib"

import {
    SFloPrint:   SFlo\$Machine -> ();
} from Foreign C;

Main():() == {
    import from Machine, SingleFloat;

    sf:SingleFloat := 3.14;
    print << "sf = " << sf << newline;
    SFloPrint(sf::SFlo);
}

Main();
EOF


cat << EOF > bug1226c.c
#include <stdlib.h>
#include <stdio.h>

void SFloPrint(float sflo) 
{
    printf("sflo = %f\n", sflo);
}
EOF

echo "Compiling the C library ..."
unicl -c bug1226c.c

echo "Running the test ..."
aldor -grun -Mno-ALDOR_W_CantUseArchive -laxllib bug1226.as bug1226c.$OBJ


echo "Cleaning up ..."
/bin/rm -f bug1226.as bug1226c.c bug1226c.$OBJ

echo "Finished!"
exit 0

