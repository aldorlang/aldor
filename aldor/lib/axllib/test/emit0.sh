#!/bin/sh
#
# This script tests the use of temporary files and warning messages in emit.c.

SRC=`pwd`
cd ${TMPDIR-/tmp}

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	P="win"
	ALDOR=aldor.sh
	OBJ=obj
	EXE=exe
	S=`cygpath -m $SRC`
	SRC=$S
	UNICL=unicl.sh
	LM=
else
	P=
	ALDOR=aldor
	OBJ=o
	EXE=
	UNICL=unicl
	LM=-lm
fi

# Generate all files.
echo '== Compiling test1.as, generating all output files, and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F ai -F ap -F fm -F ao -F asy -F lsp -F c -F o -F x -Grun $SRC/test1.as
echo '-- The files are:'
if [ "$P" = "win" ]; then 
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

# Test warning messages when no output is generated.
echo '== Compiling test1.as to test1.lsp with all other files around.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F lsp $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Compiling test1.as to test1.c with all other files around.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Compiling test1.as to test1.lsp with a non-aldor .c file around.'
rm -f test1.c
touch test1.c
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F lsp $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Compiling test1.as to test1.c with a non-aldor .lsp file around.'
rm -f test1.lsp test1.c
touch test1.lsp
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

# Test fatal errors when trying to overwrite non-generated files.
echo '== Compiling test1.as to test1.lsp, trying to overwrite non-aldor file.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F lsp $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Compiling test1.as to test1.c, trying to overwrite non-aldor file.'
rm -f test1.c
touch test1.c
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

# Test clobbering of aldor generated files.
echo '== Compiling test1.as to test1.lsp and test1.c.'
rm -f test1.lsp test1.c
aldor -Mno-ALDOR_W_CantUseArchive $LM -F lsp -F c -laxllib $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Compiling test1.as, generating all output files, and running it.'
touch TsNewer
sleep 1
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F ai -F ap -F fm -F ao -F asy -F lsp -F c -F o -F x -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
sleep 1
echo test1*
#in cygwin find does not return . if a file in the current directory is updated
touch .
sleep 2
echo '-- Newly generated files are:'
/usr/bin/find . -newer TsNewer -print | sort
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

# Test warnings and use of temporary files when not keeping the output.
echo '== Compiling test1.as and running it, not generating output files.'
touch TsNewer
sleep 1
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
sleep 1
echo test1*
echo '-- Newly generated files are:'
#in cygwin find does not return . if a file in the current directory is updated
touch .
sleep 1
/usr/bin/find . -newer TsNewer -print | sort
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Ditto, with non-aldor .lsp and .c files around.'
rm -f test1.lsp test1.c
touch test1.lsp test1.c
touch TsNewer
sleep 1
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
	mv test1.exe test1
fi
sleep 1
echo test1*
echo '-- Newly generated files are:'
#in cygwin find does not return . if a file in the current directory is updated
touch .
sleep 1
/usr/bin/find . -newer TsNewer -print | sort
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
	mv test1 test1.exe
fi

echo '== Removing all output files.'
rm -f test1.ai test1.ap test1.asy test1.ao test1.fm test1.lsp test1.c test1.obj test1.o test1.exe test1 TsNewer

# Make sure the generated C code doesn't contain a main() function.
echo '== Compiling test1.as to test1.c.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
mv test1.c test1.c.bak
echo '-- Compiling test1.as to test1.c and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- Comparing the two generated .c files.'
diff test1.c.bak test1.c
rm -f test1.c

# Make sure the generated lisp code doesn't contain a main expression.
echo '== Compiling test1.as to test1.lsp.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F lsp $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
mv test1.lsp test1.lsp.bak
echo '-- Compiling test1.as to test1.lsp and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F lsp -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- Comparing the two generated .lsp files.'
diff test1.lsp.bak test1.lsp
rm -f test1.lsp.bak test1.lsp

# Test the use of temporary files and renaming if either .c or .o is around.
echo '== Compiling test1.as and running it with test1.c around.'
mv test1.c.bak test1.c
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
echo test1*

echo '== Compiling test1.as to test1.o with test1.c around.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F o $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
fi
#mv test1 test1.exe

echo '== Compiling test1.as and running it with test1.o around.'
rm -f test1.c
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
fi

echo '== Compiling test1.as to test1.c with test1.o around.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
if [ "$P" = "win" ]; then
	mv test1.obj test1.o
fi
echo test1*
if [ "$P" = "win" ]; then
	mv test1.o test1.obj
fi

echo '== Removing all output files.'
rm -f test1.c test1.$OBJ

# Test feeding aldor generated files back into aldor.
echo '== Compiling test1.as to test1.ao.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F ao $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- Compiling test1.ao and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun test1.ao|sed "s/\.obj/.o/"|sed "s/\.exe//"
rm test1.ao

echo '== Compiling test1.as to test1.fm.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F fm $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- Compiling test1.fm and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun test1.fm|sed "s/\.obj/.o/"|sed "s/\.exe//"
rm test1.fm

echo '== Compiling test1.as to test1.o.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F o $SRC/test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- Compiling test1.o and running it.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun test1.$OBJ|sed "s/\.obj/.o/"|sed "s/\.exe//"
rm test1.$OBJ

# Test clobbering a source file.
# This test is too different under Windows so it will be skipped.
echo '== Compiling test1 to test1.'
if [ "$P" = "win" ]; then
	cp $SRC/test1.as test1.exe
cat <<Here
#1 (Fatal Error) Output would clobber input file \`test1'.
#1 (Warning) Removing file \`test1.o'.
#2 (Warning) Removing file \`aldormain.o'.
Here
	echo '-- The files are:'
	mv test1.exe test1
	echo test1*
	mv test1 test1.exe
else
	cp $SRC/test1.as test1
	$ALDOR -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F x test1
	echo '-- The files are:'
	echo test1*
fi

# This test is too different under Windows so it will be skipped.
echo '== Ditto, with -e option given.'
if [ "$P" = "win" ]; then
cat <<Here
#1 (Fatal Error) Output would clobber input file \`test1'.
#1 (Warning) Removing file \`test1.o'.
#2 (Warning) Removing file \`aldormain.o'.
Here
	echo '-- The files are:'
	mv test1.exe test1
	echo test1*
	mv test1 test1.exe
else
	$ALDOR -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F x -e test1 test1|sed "s/\.obj/.o/"|sed "s/\.exe//"
	echo '-- The files are:'
	echo test1*
fi

echo '== Compiling test1.as to test1.as.'
if [ "$P" = "win" ]; then
	mv test1.exe test1.as
else
	mv test1 test1.as
fi
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Fx=test1.as test1.as|sed "s/\.obj/.o/"|sed "s/\.exe//"
echo '-- The files are:'
echo test1*

echo '== Compiling test1.as and foo.as to foo.as.'
cp test1.as foo.as
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Fx=foo.as test1.as foo.as | grep -v "GC:"|sed "s/\.obj/.o/"
echo '-- The files are:'
echo test1* foo*

# Test funny -e options.
echo '== Compiling test1.as and foo.as to .c files with -e bar.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F c -e bar test1.as foo.as | grep -v "GC:"|sed "s/\.obj/.o/"
rm -f test1.c foo.c
echo '-- The files are:'
echo test1* foo*

echo '== Compiling test1.as and foo.as to executable with -e bar.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -F x -e bar test1.as foo.as | grep -v "GC:"|sed "s/\.obj/.o/"
echo '-- The files are:'
echo test1* foo*

echo '== Cleaning up'
rm -f test1* foo*

# Test running an aldor script.
echo '== Creating aldor script test1 and running it.'
ln -s $ALDORROOT/bin/aldor aldor
echo "-K1 -Grun -Mno-ALDOR_W_CantUseArchive $LM -laxllib" > args
echo "#!./aldor -aargs" > hdr
cat hdr $SRC/test1.as > test1
chmod +x test1
./test1
echo '-- The files are:'
echo test1*
rm -f args hdr test1 aldor

# Test file cleanup when the C compiler fails.
echo '== Creating a broken C compiler.'
echo "$ALDORROOT/bin/$UNICL \$*" > unicl.1
echo "mv -f unicl.2 unicl.new" >> unicl.1
echo "exit 0" >> unicl.1
echo "exit 1" > unicl.2
chmod +x unicl.1 unicl.2

echo '== Trying to compile and run test1.as without a C compiler.'
$ALDOR -Mno-ALDOR_W_CantUseArchive $LM -laxllib -C cc=$TMPDIR/unicl.new -Grun $SRC/test1.as 2> /dev/null | grep -v "unicl"
echo '-- The files are:'
echo test1*

#Not the same output under windows. Skip test.
echo '== Trying to compile and run test1.as with a broken C compiler.'
mv unicl.1 unicl.new
if [ "$P" = "win" ]; then
cat <<Here
#1 (Warning) Removing file \`test1.o'.
#2 (Warning) Removing file \`aldormain.c'.
Here
else
	aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -C cc=$TMPDIR/unicl.new -Grun $SRC/test1.as 2> /dev/null | grep -v "unicl"
fi
echo '-- The files are:'
echo test1*
rm -f unicl.new

echo '== Compiling and running test1.as.'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Grun $SRC/test1.as
echo '-- The files are:'
echo test1*

echo '== Interpreting an .as file - no .ao should remain'
rm -f test1.ao
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Ginterp $SRC/test1.as
if [ -f "test1.ao" ] ; then
	echo Test FAILED
else
	echo Test OK
fi

echo '== Interpreting an .as file with -Fao'
rm -f test1.ao
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Ginterp -Fao $SRC/test1.as
if [ -f "test1.ao" ] ; then
	echo Test OK
else
	echo Test FAILED
fi
rm -f test1.ao

echo '== Interpreting an .ao file'
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib $SRC/test1.as
aldor -Mno-ALDOR_W_CantUseArchive $LM -laxllib -Ginterp test1.ao
if [ -f "test1.ao" ] ; then
	echo Test OK
else
	echo Test FAILED
fi
rm -f test1.ao


echo '== Cleaning up'
rm -f test1*
