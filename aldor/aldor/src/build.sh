#!/bin/bash

#
# Source is layered with each level referring to itself or lower levels
#

# Level 4: Compiler Top Level
MainC="main.c main_t.c winmain.c"

TopH="axl.h axltop.h"
TopC="axlcomp.c cmdline.c"

# Level 3: Compiler Phases and Subordinates
PhaseY="axl.y"
PhaseH="axlphase.h gf_util.h"
PhaseC="
	phase.c bloop.c
	include.c scan.c syscmd.c linear.c parseby.c macex.c
	abnorm.c abcheck.c abuse.c scobind.c
	ti_decl.c
	tinfer.c ti_bup.c ti_tdn.c ti_sef.c terror.c
	genfoam.c gf_fortran.c gf_add.c gf_gener.c gf_imps.c
	gf_excpt.c gf_reference.c gf_implicit.c
	gf_prog.c gf_rtime.c gf_seq.c opttools.c
	optfoam.c of_util.c  of_inlin.c of_cfold.c of_hfold.c
	of_emerg.c of_env.c of_cprop.c of_jflow.c of_peep.c
	of_deadv.c of_comex.c of_loops.c of_retyp.c of_deada.c
	of_rrfmt.c of_killp.c
	emit.c ccomp.c usedef.c flatten.c
	inlutil.c genc.c genlisp.c fortran.c gencpp.c
"
# Level 2: Compiler Structures
ObjectsC="
	axlobs.c srcline.c token.c doc.c absyn.c absub.c ablogic.c
	fint.c output.c simpl.c compcfg.c depdag.c
	sefo.c syme.c freevar.c tform.c tfsat.c tposs.c tconst.c
	tqual.c stab.c lib.c archive.c foam.c flog.c dflow.c spesym.c
	abpretty.c version.c comsg.c loops.c 
"

# Level 1: General Library -- NB: Should split foam files
GeneralC="
	debug.c format.c fluid.c util.c store.c memclim.c test.c
	foam_c.c foam_i.c foam_cfp.c foamopt.c compopt.c dword.c
	bigint.c bitv.c btree.c buffer.c dnf.c file.c fname.c list.c
	msg.c path.c srcpos.c strops.c symbol.c table.c ccode.c
	sexpr.c xfloat.c priq.c timer.c cfgfile.c
	termtype.c textansi.c texthp.c textcolour.c
"
GeneralH="axlgen.h axlgen0.h editlevels.h optcfg.h"
GeneralT="
	store1_t.c store2_t.c store3_t.c format_t.c fluid_t.c util_t.c
	bigint_t.c bitv_t.c btree_t.c buffer_t.c dnf_t.c file_t.c
	fname_t.c list_t.c msg_t.c strops_t.c symbol_t.c table_t.c
	ccode_t.c xfloat_t.c link_t.c priq_t.c float_t.c
"

# Level 0: Portability
PortH="axlport.h platform.h cconfig.h"
StdcH="
	assert.h0 ctype.h0 errno.h0 float.h0 limits.h0	
	locale.h0 math.h0 setjmp.h0 signal.h0 stdarg.h0
	stddef.h0 stdio.h0 stdlib.h0 string.h0 time.h0
	unistd.h0
"
PortC="stdc.c cport.c opsys.c"
OsInc="os_cms.c os_dos.c os_os2.c os_unix.c os_vms.c
	os_macs7.c os_macosx_vm.c os_win32.c"
PortT="cport_t.c opsys_t.c"

PortA="" # any_as.s for Sun only

#
# Generated files -- keep for platforms with weak tool chains
#
MadeY="axl_y.yt axl_y.sed"
MadeC="comsgdb.c axl_y.c"
MadeH="comsgdb.h"
MadeFiles="$MadeY $MadeC $MadeH"

# Configuration files
ConfFiles="aldor.conf sample.terminfo"


#
# File collections
#

CompC="$TopC $PhaseC $ObjectsC $GeneralC $PortC"
CompH="`echo $CompC | sed -e 's/\\.c/.h/g'`"

MoreC="$MainC $MadeC $OsInc"
MoreH="$MadeH $TopH $PhaseH $GeneralH $PortH $StdcH"
TestC="$GeneralT $PortT" # Self-test files
Experimental="of_argsub.h of_argsub.c genssa.h genssa.c newjflow.c"

OKFILES="
	Makefile build.sh ChangeLog
	basic.typ  comsgdb.msg
	any_as.s $PhaseY
	$ConfFiles $MadeY
	$CompC $CompH $MoreC $MoreH $TestC $Experimental
"

#############################################################################

if [ "$1"x = "junk"x ] ; then
    for f in `ls -d $OKFILES * | sort | uniq -u` ; do echo `pwd`/$f ; done

elif [ "$1"x = "build"x ] ; then
    echo ">>> Enter `pwd`"
    if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi

    CC=gcc
    #CFLAGS="-g -DTEST_ALL -DSTO_DEBUG_DISPLAY" # -DSTO_CAN_BLACKLIST
    #CFLAGS="-O -DTEST_ALL -DUnBPack"
    #CFLAGS="-pg -DTEST_ALL -DNLOCAL"
    #CFLAGS="-O -DNDEBUG"
    #CFLAGS="-O -DNDEBUG -DTEST_ALL -DUSE_MEMORY_CLIMATE"
    #CFLAGS="-g -O3  -DNDEBUG -DTEST_ALL"
    CFLAGS="-g -O3  -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"

    AR=ar
    ARFLAGS=r
    if uname -a | grep Cygwin >/dev/null ; then EXE=exe ; else EXE=out ; fi

    function buildlib () {
	    Lib=$1; shift
	    rm -f $Lib
	    for SrcFile in $* ; do
		echo "Compiling $SrcFile"
		ObjFile=`echo $SrcFile | sed -e 's/\\.[cs]/.o/'`
		$CC $CFLAGS -c $SrcFile
		$AR $ARFLAGS $Lib $ObjFile
		rm $ObjFile
	    done
	    ranlib $Lib
	    mv $Lib $ALDORROOT/lib
    }
    
    # Ensure target directories exist
    echo "--- target directories"
    mkdir -p $ALDORROOT/include
    mkdir -p $ALDORROOT/bin
    mkdir -p $ALDORROOT/lib

    # The include directory
    echo "--- include directorys"
    cp basic.typ       $ALDORROOT/include
    cp aldor.conf      $ALDORROOT/include
    cp sample.terminfo $ALDORROOT/include

    # Grammar
    echo "--- grammar"
    $ALDORROOT/toolbin/zacc -p -y axl_y.yt -c axl_y_temp.c axl.y
    sed -f axl_y.sed axl_y_temp.c > axl_y.c
    rm axl_y_temp.c

    # Libraries
    echo "--- library"
    buildlib libascomp.a \
	$TopC $PhaseC $ObjectsC $MadeC \
	$GeneralC $PortC $TestC $PortA

    # Main programs
    echo "--- aldor_t"
    $CC $CFLAGS main_t.c -L$ALDORROOT/lib -lascomp -lm
    mv a.$EXE $ALDORROOT/bin/aldor_t

    echo "--- aldor"
    $CC $CFLAGS main.c   -L$ALDORROOT/lib -lascomp -lm
    mv a.$EXE $ALDORROOT/bin/aldor

    # Message databases
    echo "--- message databases"
    $ALDORROOT/bin/msgcat -h -c -detab comsgdb
    cp comsgdb.msg $ALDORROOT/lib

    $ALDORROOT/toolbin/atinlay <comsgdb.msg >comsgpig.msg
    $ALDORROOT/bin/msgcat -cat -detab comsgpig
    mv comsgpig.msg $ALDORROOT/lib
    rm comsgpig.*

    # Test for bad zeros
    echo "--- bad zeros"
    $ALDORROOT/toolbin/badzeros $CompC $TestC $CompH | \
	 grep -v foamopt.h | grep -v compopt.h

    echo "<<< Exit `pwd`"

fi
