#!/bin/sh
#
# This script tests using a message file.

if [ "$TMPDIR" = "" ] ; then
	TMPDIR=/tmp
fi

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	SEP="\\"
	ALDOR=aldor.sh
else
	P=
	SEP="/"
	ALDOR=aldor
fi

cat > $TMPDIR/junk.as <<EOF
  -- We will use this to generate lots of messages.
  a b c d e f g h i j k l m n o p q r
  s t u v w x y z 0 1 2 3 4 5 6 7 8 9
EOF

if [ "$P" = "win" ]; then
	unix2dos $TMPDIR/junk.as > /dev/null 2>&1 
fi

AXIOMXLARGS=-M2
export AXIOMXLARGS

echo '== No files -- gives hint.'
aldor

echo '== Bad option -- gives hint.'
aldor -.

echo '== Bad use of option -- gives hint.'
aldor -R

echo '== Bad developer option -- gives hint.'
aldor -WX

echo '== Message file.'
aldor -M db=comsgpig -F

echo '== Message on line no newline at end.'
(cd $TMPDIR ; echon "3 + 3" > nonl.as ; aldor nonl.as)

echo '== Turn on message normally off.'
$ALDOR -R $TMPDIR -M ALDOR_R_StabImporting test0.as

echo '== Turn off message normally on.'
aldor -M no-ALDOR_W_NoFiles

echo '== Test remarks =='
$ALDOR -R $TMPDIR test0.as

echo '== Turn on all remarks.'
if [ "$P" = "win" ]; then
	aldor.sh -R $TMPDIR -M remarks test0.as | grep -v '\\include\\'
else
	$ALDOR -R $TMPDIR -M remarks test0.as | grep -v "/include/"
fi

echo '== Turn on all remarks but one.'
if [ "$P" = "win" ]; then
	aldor.sh -R $TMPDIR -M remarks -M no-ALDOR_R_StabImporting test0.as | grep -v '\\include\\'
else
	aldor -R $TMPDIR -M remarks -M no-ALDOR_R_StabImporting test0.as | grep -v "/include/"
fi

echo '== Test warnings =='
$ALDOR -R $TMPDIR -WTt+scan scan0.as

echo '== Turn off all warnings (1).'
aldor -M no-warnings

echo '== Turn off all warnings (2).'
$ALDOR -R $TMPDIR -M no-warnings -WTt+scan scan0.as

echo '== Turn off all warnings but one.'
$ALDOR -R $TMPDIR -M no-warnings -M ALDOR_W_SysCmdUnknown -WTt+syscmd scan0.as

echo '== Do not turn off errors.'
$ALDOR -R $TMPDIR -D TestErrorsToo -M no-ALDOR_E_TinExprMeans test0.as

echo '== Complain about non-existent messages.'
aldor -M ALDOR_R_YouAintGonnaFindIt

echo '== Too many messages'
(cd $TMPDIR ; aldor junk.as )

echo '== Limit number of messages'
(cd $TMPDIR ; aldor -M emax=3 junk.as )

echo '== Not too many messages'
(cd $TMPDIR ; aldor -M no-emax junk.as )

echo '== Help message.'
aldor -hall|sed "s/\.obj/.o  /"

echo '== Cleaning up.'
rm -f $TMPDIR/test0.ao $TMPDIR/ovload0.as* $TMPDIR/nonl.as $TMPDIR/junk.as
