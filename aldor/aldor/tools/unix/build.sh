#!/bin/bash
    
OKFILES="
	Makefile build.sh
	aldoc2html.c aldorbug atinlay.c axiomxl badzeros.c 
	buildarg ccode.h cenum.h cenum.c cids collide.c
	cparse.h cparse.y cprt.awk cscan.h cscan.c cwords.c dirname.c diskchek.c 
	doaldor docc dog++ dolatex domkmk doranlib dosfile.c dosify 
	echon.c exclude.c extract.c fixbug fixreply flags.h flags.c 
	getbug krcc macify makemake.c memlay.c mkasys mklib.sh 
	mksrctex msgcat.c oldc.c shorten.c skimenum.c strarray.c 
	txt2txt.c undent unixify 
	zacc.h zacc.c zcport.h zaccgram.y zaccscan.l
"
UNSUREFILES="
	doas doaxiomxl dopdflatex includes.c 
	maxiomxl.c maxiomxl.sh memclean.c older24h.c realuser.c suru.c
"

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES $UNSUREFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
    echo ">>> Enter `pwd`"

    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi
    
    CC=gcc
    CFLAGS="-O -ansi"

    LEX=flex
    YACC=yacc
    
    function announce () {
    	echo Building $1
    }
    function install () {
    	announce $2
    	mkdir -p $ALDORROOT/$1
    	cp    $2 $ALDORROOT/$1/$2
    	chmod 755 $ALDORROOT/$1/$2
    }
    function installc () {
    	announce $2
    	mkdir -p $ALDORROOT/$1
        dir=$1 ;    shift
    	target=$1 ; shift
    	$CC $CFLAGS $target.c $* -o $ALDORROOT/$dir/$target
    	chmod 755 $ALDORROOT/$dir/$target
    }
    
    # Tools delivered to end-user [bin directory]
    
    installc bin msgcat             # Construct an X/Open message catalog together with .c and .h files
    install  bin mklib.sh
    install  bin aldorbug           # Mail an Aldor bug report
    installc bin extract flags.c    # Extracts tests from source files
    installc bin aldoc2html flags.c # Take aldoc comments and create an html file
    
    # Other tools [toolbin directory]
    installc toolbin echon
    installc toolbin txt2txt    # Convert between various os file formats.
    install  toolbin axiomxl
    install  toolbin dosify
    install  toolbin macify
    install  toolbin unixify
    
    install  toolbin domkmk     # Platform-dependent interface to makemake
    installc toolbin makemake   # Construct a make file based on include commands.
    installc toolbin skimenum cenum.c # Extract enumeration values from a C source file.
    installc toolbin exclude    # Undo cpp includes.
    installc toolbin strarray   # Construct .c and .h files for an array of the file lines.
    installc toolbin dirname    # Give directory part of file name, for portable scripts.
    installc toolbin atinlay    # Translate a file to pig latin.
    installc toolbin memlay     # Explore machine's memory layout
    installc toolbin diskchek   # Check that distribution disk images fit on real diskettes.
    installc toolbin dosfile    # Convert between dos and unix file formats.
    install  toolbin undent     # Convert file to standard indentation -- user to touch up.
    installc toolbin badzeros cscan.c # Locate suspicious zeros in a C source file.
    install  toolbin doaldor    # Helper script for compiling aldor library files.
    install  toolbin dolatex    # Platform customization for "latex".
    install  toolbin doranlib   # Platform customization for "ranlib".
    install  toolbin buildarg   # buildarg
    install  toolbin docc       # Platform customization for "cc".
    install  toolbin dog++      # Platform customization for "g++".
    install  toolbin krcc       # Convert a program to K+R C, then compile.
    install  toolbin mksrctex   # Latex axiomxl compiler source with index.
    install  toolbin cprt.awk   # called by mksrctex
    install  toolbin cids       # Produce duplicate-free set of ids from set of C files.
    installc toolbin collide    # Find words which are the same up to a given length.
    installc toolbin shorten    # Produce a file of #define-s to rename identifiers.
    installc toolbin cwords cscan.c # Extract all identifiers from a C source file.
    install  toolbin getbug     # Request a reported bug
    install  toolbin fixbug     # Save a bug fix
    install  toolbin fixreply   # Report bug fix to bug reporter
    install  toolbin mkasys     # dump .asy files from all .ao's in libaxllib.al
    
    # zacc: Parser generator with parameterized rules. Based on Yacc.
    announce zacc
    $LEX zaccscan.l
    $YACC -d zaccgram.y 
    mv y.tab.h zaccgram.h
    $CC $CFLAGS zacc.c lex.yy.c y.tab.c cenum.c -o $ALDORROOT/toolbin/zacc
    rm lex.yy.c y.tab.c zaccgram.h
    
    # oldc: Convert ANSI C-file to K+R C.
    announce oldc
    $ALDORROOT/toolbin/zacc -p -c cparse.c cparse.y 2>& 1 | \
	grep -v "1 shift/reduce conflict"
    $CC $CFLAGS -DYYMAXDEPTH=500 oldc.c cscan.c cparse.c \
	-o $ALDORROOT/toolbin/oldc
    rm cparse.c

    echo "<<< Exit `pwd`"
fi
