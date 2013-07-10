##############################################################################
#
# Makefile
#
# Make program to compile the Foam runtime (frisco) files
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
#
##############################################################################
#
# Targets: all, junk, clean, quick
#
#    make all	-- make the runtime files for the Aldor compiler
#    make junk	-- list files which are not part of the Aldor compiler
#    make clean -- remove generated files  (e.g. foo.c~)
#
# Parameters:  ALDORROOT
#
#    To override defaults do, e.g.,
#	 make -e ALDORROOT=/spad/local/aldor/rs all
#    or
#	 setenv ALDORROOT /spad/local/aldor/rs; setenv MAKEFLAGS e; make all
#
##############################################################################
#
# We do not use the compiler archive since these files may be compiled
# with different options, or different compilers.
#
##############################################################################
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL	= /bin/sh
AR	= ar rv
CP	= cp -p
LN	= ln
RM	= rm -f
OBJ	= o
LIB	= a

SYS      = `echo $(MACHINE) | awk -F- '{print $$1}'`


ifeq "$(findstring sun4os57g,$(MACHINE))" "sun4os57g"
  CCPROG=docc
else
  CCPROG=unicl -g $(UNICLARGS) -Wstdc -Wsys=$(SYS)
endif 

ALDOR=$(ALDORROOT)/bin/aldor -fo -Csmax=0 

ifeq ($(MACHINE), win32gcc)
	CCPROG	= unicl.sh $(UNICLARGS) -Wstdc -Wsys=$(SYS)
	ALDOR	= $(ALDORROOT)/bin/aldor.sh -fo -Csmax=0 
endif

ifeq ($(MACHINE), win32msvc)
	CCPROG	= unicl.sh $(UNICLARGS) -Wstdc -Wsys=$(SYS)
	ALDOR	= $(ALDORROOT)/bin/aldor.sh -fo -Csmax=0 
	OBJ	= obj
	LIB	= lib
endif

ifeq "$(findstring rs,$(MACHINE))" "rs"
  GMPSCRIPT=aldor.gmp.bsh
else
  GMPSCRIPT=aldor.gmp.sh
endif 

GMP2 = gmp-2.$(LIB)
GMP3 = gmp-3.$(LIB)

CPPPROG  = g++ $(CPPARGS)
CPPFLAGS = -DNDEBUG -DFOAM_RTS  -O3 
# CFLAGS	 =  -DNDEBUG -DFOAM_RTS -O # -DSTO_CAN_BLACKLIST
DFLAGS	 =  # -DSTO_DEBUG_DISPLAY -DSTO_CENSUS # -DSTO_SHOW
OFLAGS	 =  -O -DNDEBUG #-Wopts=-pg # -g

ifeq ("$(MACHINE)", "win32gcc")
	CFLAGS	 =  -DFOAM_RTS $(DFLAGS) $(OFLAGS)
#	CFLAGS	 =  -DFOAM_RTS -g
else
	CFLAGS	 =  -DFOAM_RTS $(DFLAGS) $(OFLAGS)
#	CFLAGS	 =  -DFOAM_RTS -g
endif 

#
# Environment
#

IncDir	= $(ALDORROOT)/include
LibDir	= $(ALDORROOT)/lib
BinDir  = $(ALDORROOT)/bin

SRC     =../../src
CmmDir  =../cmm

BUILDARG = $(ALDORROOT)/toolbin/buildarg
MACH	 = `echo $(MACHINE) | awk -F- '{print $$1}'`

LIBFOAMAL= $(LibDir)/libfoam.al
LIBFOAM	= $(LibDir)/libfoam-gmp.$(LIB)
BUILDARG = $(ALDORROOT)/toolbin/buildarg

AXLCDB = # -Cargs=-Wopts=-pg # -O
ALDORARGS = -Q3 -Qinline-all -M no-ALDOR_W_OverRideLibraryFile $(AXLCDB) # -O

ifeq "$(GmpDir)" ""
  GmpDir	= /usr/include
endif

#GmpDir	= /users/axiom/gmp/gmp-3.1.1
#GmpDir	= /users/axiom/gmp/gmp-3.0
#GmpDir	= /usr/packages/gmp-3.0
#GmpDir	= /usr/packages/gmp-2.0.2

#
# Files from compiler sources
#

PORTH0        = axlport.h  platform.h cconfig.h optcfg.h foamopt.h compopt.h 
STDCH0        = assert.h0 ctype.h0 errno.h0  float.h0  limits.h0 \
		locale.h0 math.h0  setjmp.h0 signal.h0 stdarg.h0 \
		stddef.h0 stdio.h0 stdlib.h0 string.h0 time.h0 unistd.h0
OPSYSH0       = os_cms.c os_dos.c os_os2.c os_unix.c os_vms.c \
		os_macs7.c os_macosx_vm.c os_win32.c
FOAMHX        =	axlgen.h axlgen0.h editlevels.h debug.h fluid.h format.h \
		test.h memclim.h

FOAMC	      = stdc.c   cport.c   	\
		opsys.c  btree.c 	\
		util.c   foam_c.c 	\
		output.c table.c 	\
		xfloat.c foamopt.c 	\
		dword.c  compopt.c 	\
		store.c  bigint.c	\
		timer.c  foam_cfp.c

FOAMCC 	      =	fm_gmp.c
LOCALH	      = 

FOAMH         = $(PORTH0) $(STDCH0) $(OPSYSH0)  \
		$(FOAMC:.c=.h) $(FOAMHX) 

COMMONFILES   = $(FOAMH) $(FOAMC)
OKFILES       = Makefile aldor.gmp.sh  aldor.gmp.bsh $(FOAMC) $(FOAMCC) $(LOCALH) $(FOAMH)

OBJECTS       = $(LIBFOAM)(stdc.$(OBJ))   $(LIBFOAM)(cport.$(OBJ))   \
		$(LIBFOAM)(opsys.$(OBJ))  $(LIBFOAM)(btree.$(OBJ))   \
		$(LIBFOAM)(util.$(OBJ))   $(LIBFOAM)(foam_c.$(OBJ))  \
		$(LIBFOAM)(output.$(OBJ)) $(LIBFOAM)(table.$(OBJ))   \
		$(LIBFOAM)(xfloat.$(OBJ)) $(LIBFOAM)(foamopt.$(OBJ)) \
		$(LIBFOAM)(dword.$(OBJ))  $(LIBFOAM)(compopt.$(OBJ)) \
		$(LIBFOAM)(store.$(OBJ))  $(LIBFOAM)(bigint.$(OBJ))  \
		$(LIBFOAM)(timer.$(OBJ))  $(LIBFOAM)(foam_cfp.$(OBJ)) \
		$(LIBFOAM)(runtime.$(OBJ))			   \
		$(LIBFOAM)(fm_gmp.$(OBJ))

# General Rules

all: .cvsignore lib extlibs script

clean: 
	@ $(RM) *~
	@ $(RM) $(FOAMH) $(FOAMC)

junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

extlibs:
	for x in `$(BUILDARG) extra-libdirs` ; do 		\
		lib=$(ALDORROOT)/lib/$(MACH)$$x;		\
		$(MAKE) lib LibDir=$$lib SYS=$(MACH)$$x ;	\
	done

$(LibDir): 
	mkdir $(LibDir)

lib: $(LibDir) $(COMMONFILES) $(LIBFOAM)

script: $(ALDORROOT)/bin/aldor.gmp

$(ALDORROOT)/bin/aldor.gmp: aldor.gmp.bsh aldor.gmp.sh
	cp ${GMPSCRIPT} $@
	chmod 0777 $@

# The following can be used to obtain a system on which both GMP-2 and
# GMP-3 can be used together. After building libfoam-gmp.a with GmpDir
# set to the GMP-2 installation, rename it to libfoam-gmp2.a. Change
# GmpDir here to point to the GMP-3 installation, recompile and copy
# to libfoam-gmp3.a (all in $AXIOMXROOT/lib of course).
#
# Then `make extras' to finish off: you may need to change the
# path names for GMP-2 and GMP-3 below for your system.
extras:
	/bin/rm -f $(ALDORROOT)/lib/lib$(GMP2)
	/bin/rm -f $(ALDORROOT)/lib/lib$(GMP3)
	/bin/rm -f $(ALDORROOT)/bin/aldor.gmp2
	/bin/rm -f $(ALDORROOT)/bin/aldor.gmp3
	ln -s /usr/local/lib/libgmp-2.0.2.a $(ALDORROOT)/lib/lib$(GMP2)
	ln -s /usr/local/lib/libgmp-3.0.a $(ALDORROOT)/lib/lib$(GMP3)
	cp aldor.gmp2 $(ALDORROOT)/bin
	cp aldor.gmp3 $(ALDORROOT)/bin
	chmod 755 $(ALDORROOT)/bin/aldor.gmp2
	chmod 755 $(ALDORROOT)/bin/aldor.gmp3

.cvsignore: Makefile
	echo .cvsignore $(COMMONFILES) > .cvsignore

samples:

#
# Rules to get the common source files (as links or copies)
#
axlport.h:	$(SRC)/axlport.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
platform.h:	$(SRC)/platform.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
cconfig.h:	$(SRC)/cconfig.h ;	 ( $(RM) $@ ; $(LN) $? $@ )

assert.h0:	$(SRC)/assert.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
ctype.h0:	$(SRC)/ctype.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
errno.h0:	$(SRC)/errno.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
float.h0:	$(SRC)/float.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
limits.h0:	$(SRC)/limits.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
locale.h0:	$(SRC)/locale.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
math.h0:	$(SRC)/math.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
setjmp.h0:	$(SRC)/setjmp.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
signal.h0:	$(SRC)/signal.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
stdarg.h0:	$(SRC)/stdarg.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
stddef.h0:	$(SRC)/stddef.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
stdio.h0:	$(SRC)/stdio.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
stdlib.h0:	$(SRC)/stdlib.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
string.h0:	$(SRC)/string.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
time.h0:	$(SRC)/time.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )
unistd.h0:	$(SRC)/unistd.h0 ;	 ( $(RM) $@ ; $(LN) $? $@ )

os_cms.c:	$(SRC)/os_cms.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_dos.c:	$(SRC)/os_dos.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_os2.c:	$(SRC)/os_os2.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_unix.c:	$(SRC)/os_unix.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_vms.c:	$(SRC)/os_vms.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_macs7.c:	$(SRC)/os_macs7.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_macosx_vm.c:	$(SRC)/os_macosx_vm.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
os_win32.c:	$(SRC)/os_win32.c ;	 ( $(RM) $@ ; $(LN) $? $@ )

axlgen.h:	$(SRC)/axlgen.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
axlgen0.h:	$(SRC)/axlgen0.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
editlevels.h:	$(SRC)/editlevels.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
debug.h:	$(SRC)/debug.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
fluid.h:	$(SRC)/fluid.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
format.h:	$(SRC)/format.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
test.h: 	$(SRC)/test.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
memclim.h: 	$(SRC)/memclim.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )

stdc.h: 	$(SRC)/stdc.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
cport.h:	$(SRC)/cport.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
opsys.h:	$(SRC)/opsys.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
btree.h:	$(SRC)/btree.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
bigint.h:	$(SRC)/bigint.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
dword.h:	$(SRC)/dword.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
util.h: 	$(SRC)/util.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
table.h: 	$(SRC)/table.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
store.h: 	$(SRC)/store.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
xfloat.h: 	$(SRC)/xfloat.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
timer.h: 	$(SRC)/timer.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )

stdc.c: 	$(SRC)/stdc.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
cport.c:	$(SRC)/cport.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
opsys.c:	$(SRC)/opsys.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
btree.c:	$(SRC)/btree.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
bigint.c:	$(SRC)/bigint.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
dword.c:	$(SRC)/dword.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
util.c: 	$(SRC)/util.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
table.c: 	$(SRC)/table.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
xfloat.c: 	$(SRC)/xfloat.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
timer.c: 	$(SRC)/timer.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )

foam_c.h: 	$(SRC)/foam_c.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
foam_cfp.h: 	$(SRC)/foam_cfp.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
foamopt.h: 	$(SRC)/foamopt.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
compopt.h: 	$(SRC)/compopt.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
optcfg.h: 	$(SRC)/optcfg.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
output.h: 	$(SRC)/output.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )

foam_c.c: 	$(SRC)/foam_c.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
foam_cfp.c: 	$(SRC)/foam_cfp.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
foamopt.c: 	$(SRC)/foamopt.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
compopt.c: 	$(SRC)/compopt.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
output.c: 	$(SRC)/output.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
store.c: 	$(SRC)/store.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )


#
# Rules to compile the common files
#

# The doranlib dependency ensures that doranlib gets called.
$(LIBFOAM): $(OBJECTS) 
	doranlib $@

$(LIBFOAM)(runtime.$(OBJ)): $(LIBFOAMAL)
	cd $(LibDir); 				\
	ar x $(LIBFOAMAL) runtime.ao;		\
	$(ALDOR) $(ALDORARGS) runtime.ao;	\
	$(AR) $(LIBFOAM) runtime.$(OBJ);		\
	$(RM) runtime.$(OBJ) runtime.ao

$(LIBFOAM)(stdc.$(OBJ)):   stdc.c   $(PORTH0) $(STDCH0)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/stdc.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/stdc.c ;  \
	  $(AR) $(LIBFOAM) stdc.$(OBJ) ; \
	  $(RM) stdc.$(OBJ))

$(LIBFOAM)(cport.$(OBJ)):  cport.c  $(PORTH0) $(STDCH0)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDIr) $$SRC/cport.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/cport.c ;  \
	  $(AR) $(LIBFOAM) cport.$(OBJ) ; \
	  $(RM) cport.$(OBJ))

$(LIBFOAM)(opsys.$(OBJ)):  opsys.c  $(PORTH0) $(STDCH0) $(OPSYSH0)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/opsys.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/opsys.c ;  \
	  $(AR) $(LIBFOAM) opsys.$(OBJ) ; \
	  $(RM) opsys.$(OBJ))

$(LIBFOAM)(btree.$(OBJ)):  btree.c  $(FOAMH)
	(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/btree.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/btree.c ;  \
	  $(AR) $(LIBFOAM) btree.$(OBJ) ; \
	  $(RM) btree.$(OBJ))

store $(LIBFOAM)(store.$(OBJ)):  store.c  $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/store.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/store.c ;  \
	  $(AR) $(LIBFOAM) store.$(OBJ) ; \
	  $(RM) store.$(OBJ))

$(LIBFOAM)(bigint.$(OBJ)): bigint.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/bigint.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/bigint.c ;  \
	  $(AR) $(LIBFOAM) bigint.$(OBJ) ; \
	  $(RM) bigint.$(OBJ))

$(LIBFOAM)(dword.$(OBJ)): dword.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/dword.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/dword.c ;  \
	  $(AR) $(LIBFOAM) dword.$(OBJ) ; \
	  $(RM) dword.$(OBJ))

$(LIBFOAM)(util.$(OBJ)):   util.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/util.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/util.c ;  \
	  $(AR) $(LIBFOAM) util.$(OBJ) ; \
	  $(RM) util.$(OBJ))

$(LIBFOAM)(table.$(OBJ)):   table.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/table.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/table.c ;  \
	  $(AR) $(LIBFOAM) table.$(OBJ) ; \
	  $(RM) table.$(OBJ))

$(LIBFOAM)(xfloat.$(OBJ)):   xfloat.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/xfloat.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/xfloat.c ;  \
	  $(AR) $(LIBFOAM) xfloat.$(OBJ) ; \
	  $(RM) xfloat.$(OBJ))

$(LIBFOAM)(timer.$(OBJ)):   timer.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/timer.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/timer.c ;  \
	  $(AR) $(LIBFOAM) timer.$(OBJ) ; \
	  $(RM) timer.$(OBJ))

foam_c $(LIBFOAM)(foam_c.$(OBJ)): foam_c.c $(FOAMH)
	@(SRC=`pwd` ; FINC=`pwd` ; \
	  cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$FINC  $$SRC/foam_c.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$FINC  $$SRC/foam_c.c ;  \
	  $(AR) $(LIBFOAM) foam_c.$(OBJ) ; \
	  $(RM) foam_c.$(OBJ))

foam_cfp $(LIBFOAM)(foam_cfp.$(OBJ)): foam_cfp.c $(FOAMH)
	@(SRC=`pwd` ; FINC=`pwd` ; \
	  cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$FINC  $$SRC/foam_cfp.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$FINC  $$SRC/foam_cfp.c ;  \
	  $(AR) $(LIBFOAM) foam_cfp.$(OBJ) ; \
	  $(RM) foam_cfp.$(OBJ))

output $(LIBFOAM)(output.$(OBJ)): output.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/output.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/output.c ;  \
	  $(AR) $(LIBFOAM) output.$(OBJ) ; \
	  $(RM) output.$(OBJ))

foamopt $(LIBFOAM)(foamopt.$(OBJ)): foamopt.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/foamopt.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/foamopt.c ;  \
	  $(AR) $(LIBFOAM) foamopt.$(OBJ) ; \
	  $(RM) foamopt.$(OBJ))

compopt $(LIBFOAM)(compopt.$(OBJ)): compopt.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(IncDir) $$SRC/compopt.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(IncDir)  $$SRC/compopt.c ;  \
	  $(AR) $(LIBFOAM) compopt.$(OBJ) ; \
	  $(RM) compopt.$(OBJ))

fm_gmp $(LIBFOAM)(fm_gmp.$(OBJ)): fm_gmp.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$(GmpDir)/include -I$(IncDir) $$SRC/fm_gmp.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$(GmpDir)/include -I$(IncDir)  $$SRC/fm_gmp.c ;  \
	  $(AR) $(LIBFOAM) fm_gmp.$(OBJ) ; \
	  $(RM) fm_gmp.$(OBJ))
