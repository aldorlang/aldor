#!/bin/sh
#
# This script tests using response files.

if [ "$TMPDIR" = "" ] ; then
        TMPDIR=/tmp
fi

if [ -x /usr/5bin/echo ] ; then
        echocont=/usr/5bin/echo
else
        echocont=echo
fi

cp triv1.as $TMPDIR
cd $TMPDIR

# Create awk script to clean up -v output to just option list


if [ "`platform | sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
cat > verbclean.awk <<EOF
/^Exec: .*aldor\.exe/    {
    for (i = 1; i <= NF; i++) {
	if (i == 1) 
		printf "%s", \$i
	else if (i == 2)
		printf " aldor"
	else
		printf " %s", \$i
    }
    printf "\n"
    }
EOF
else
cat > verbclean.awk <<EOF
/^Exec: aldor/    {
    td = "$TMPDIR"
    pos = index(\$0, td)
    if (pos > 0)
        printf "%s\$TMPDIR%s\n", substr(\$0,1,pos-1), substr(\$0,pos+length(td))
    else
        print
    next
    }

            {next}
EOF
fi

echo '== Missing response files'
aldor -a
aldor -va

echo '== Empty response files'
cat > empty.as <<EOF
EOF
aldor -laxllib -a empty.as
aldor -laxllib -Oa empty.as
aldor -laxllib -K9 -vOa empty.as empty.as | awk -f verbclean.awk
rm -f empty.as *.ao

echo '== Non-empty response files'
cat > nonempty.arf <<EOF
-v triv1.as
EOF
aldor -laxllib -a nonempty.arf | awk -f verbclean.awk
aldor -lm -laxllib -Grun -a nonempty.arf 2>&1 | grep -v 'warning: conflicting types for built-in function' | awk -f verbclean.awk
# gc is dropping into an infinite loop with trailing shell args
#aldor -anonempty.arf -O | awk -f verbclean.awk
rm -f nonempty.arf *.ao

echo '== Multiple response files'
cat > respA.arf <<EOF
-v
-lm
-G run
EOF
cat > respB.arf <<EOF
-D HiSteve
-U HowdySam
triv1.as
EOF
aldor -laxllib -a respA.arf -arespB.arf 2>&1 | grep -v 'warning: conflicting types for built-in function' | awk -f verbclean.awk
rm -f respA.arf respB.arf

echo '== Nested response files'
cat > respA.arf <<EOF
-v
-lm
-Grun
-a respB.arf
EOF
cat > respB.arf <<EOF
-D HiSteve
-U HowdySam
triv1.as
EOF
aldor -laxllib -a respA.arf 2>&1 | grep -v 'warning: conflicting types for built-in function' | awk -f verbclean.awk
rm -f respA.arf respB.arf *.ao

echo '== "a" inside a bad argument does not imply a response file'
aldor -laxllib -Targets foo.as

echo '== ALDORARGS environment variable'
echo >zippo.as

ALDORARGS='-v -Fc -laxllib'
export ALDORARGS

aldor zippo.as | awk -f verbclean.awk
ls zippo.*
rm -f zippo.c

cat > zippo.arf <<EOF
-Fc
EOF

ALDORARGS='-laxllib -vazippo.arf'
export ALDORARGS

aldor -Flsp zippo.as | awk -f verbclean.awk
ls zippo.*
rm -f zippo.as zippo.arf zippo.lsp zippo.c
