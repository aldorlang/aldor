rem #!/bin/bsh
if not defined GmpDir (
	echo "no GmpDir defined -- the linker will fail unless the GMP library is in a well-known place"
	dir="" 
) else (
	dir="-Y $GmpDir"
)
$ALDORROOT\bin\aldor $dir -Cruntime=foam-car,BigInt,gmp,stdc++,cmm $*
