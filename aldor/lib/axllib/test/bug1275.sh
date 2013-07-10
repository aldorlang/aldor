#!/bin/sh
#
# Original by Saul Youssef

cd ${TMPDIR}


# Create the source for the test.
cat <<EOF > bug1275.as
#include "axllib"
#pile

Foo:(SingleInteger)->with == (i:SingleInteger):with +-> Integer
EOF


# Run the test.
echo "Interpreting the test ... "
aldor -ginterp bug1275.as
echo "      ... done. (exit status = $?)"
echo ""
echo ""


# Clean up
/bin/rm -f bug1275.as
exit 0

