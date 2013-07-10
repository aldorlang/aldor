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
#    make all	-- make the runtime files for the AXIOM-XL compiler
#    make junk	-- list files which are not part of the AXIOM-XL compiler
#    make clean -- remove generated files  (e.g. foo.c~)
#
# Parameters:  ALDORROOT
#
#    To override defaults do, e.g.,
#	 make -e ALDORROOT=/spad/local/axiomxl/rs all
#    or
#	 setenv ALDORROOT /spad/local/axiomxl/rs; setenv MAKEFLAGS e; make all
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

ifeq "$(findstring sun4os57g,$(MACHINE))" "sun4os57g"
  CCPROG=docc
else
  CCPROG=unicl -g -Wstdc $(UNICLARGS)
endif 

ALDOR=$(ALDORROOT)/bin/aldor -fo -Csmax=0 

ifeq ($(MACHINE), win32gcc)
	CCPROG	= unicl.sh -Wstdc $(UNICLARGS)
	ALDOR	= $(ALDORROOT)/bin/aldor.sh -fo -Csmax=0 
endif

ifeq ($(MACHINE), win32msvc)
	CCPROG	= unicl.sh -Wstdc $(UNICLARGS)
	OBJ	= obj
	LIB	= lib
	ALDOR	= $(ALDORROOT)/bin/aldor.sh -fo -Csmax=0 
endif

CPPPROG  = dog++ $(CPPARGS)
CPPFLAGS = -DNDEBUG -DFOAM_RTS  

ifeq "$(SYS)" ""
 SYSARG=
else
 SYSARG=-Wsys=$(SYS)
endif 
CFLAGS  = -O -DNDEBUG -DFOAM_RTS $(SYSARG)


#
# Environment
#

IncDir	= $(ALDORROOT)/include
LibDir	= $(ALDORROOT)/lib
BinDir  = $(ALDORROOT)/bin

SRC     =../../../src
CmmDir  =../cmm

LIBFOAMAL= $(LibDir)/libfoam.al
LIBFOAM	= $(LibDir)/libfoam-car.$(LIB)
BUILDARG = $(ALDORROOT)/toolbin/buildarg

ALDORARGS = -O

ifeq "$(GmpDir)" ""
  GmpDir = /cygdrive/u/gmp4
#GmpDir	= /users/axiom/gmp/gmp-3.1.1
endif

#
# Files from compiler sources
#

PORTH0        = axlport.h  platform.h cconfig.h optcfg.h foamopt.h compopt.h
STDCH0        = assert.h0 ctype.h0 errno.h0  float.h0  limits.h0 \
		locale.h0 math.h0  setjmp.h0 signal.h0 stdarg.h0 \
		stddef.h0 stdio.h0 stdlib.h0 string.h0 time.h0 unistd.h0

OPSYSH0       = os_cms.c os_dos.c os_os2.c os_unix.c os_vms.c \
		os_macs7.c os_macosx_vm.c os_win32.c
FOAMHX        =	axlgen.h axlgen0.h debug.h fluid.h format.h test.h \
		bigint.h editlevels.h

FOAMC	      = stdc.c   cport.c   	\
		opsys.c  btree.c 	\
		util.c   foam_c.c foam_cfp.c 	\
		output.c table.c 	\
		xfloat.c foamopt.c 	\
		dword.c  compopt.c 	

LOCALH	      = store.h

FOAMH         = $(PORTH0) $(STDCH0) $(OPSYSH0)  \
		$(FOAMC:.c=.h) $(FOAMHX) 

COMMONFILES   = $(FOAMH) $(FOAMC)
OKFILES       = Makefile $(LOCALH) $(COMMONFILES)

OBJECTS       = $(LIBFOAM)(stdc.$(OBJ))   $(LIBFOAM)(cport.$(OBJ))   \
		$(LIBFOAM)(opsys.$(OBJ))  $(LIBFOAM)(btree.$(OBJ))   \
		$(LIBFOAM)(util.$(OBJ))   $(LIBFOAM)(foam_c.$(OBJ)) $(LIBFOAM)(foam_cfp.$(OBJ))  \
		$(LIBFOAM)(output.$(OBJ)) $(LIBFOAM)(table.$(OBJ))   \
		$(LIBFOAM)(xfloat.$(OBJ)) $(LIBFOAM)(foamopt.$(OBJ)) \
		$(LIBFOAM)(dword.$(OBJ))  $(LIBFOAM)(compopt.$(OBJ)) \
		$(LIBFOAM)(runtime.$(OBJ))

# General Rules

all: .cvsignore library

clean: 
	@ $(RM) *~

junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

library: $(COMMONFILES) $(LIBFOAM)

.cvsignore: Makefile
	echo .cvsignore $(COMMONFILES) > .cvsignore
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
editlevels.h:	$(SRC)/editlevels.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
axlgen0.h:	$(SRC)/axlgen0.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
debug.h:	$(SRC)/debug.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
fluid.h:	$(SRC)/fluid.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
format.h:	$(SRC)/format.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
test.h: 	$(SRC)/test.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )

stdc.h: 	$(SRC)/stdc.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
cport.h:	$(SRC)/cport.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
opsys.h:	$(SRC)/opsys.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
btree.h:	$(SRC)/btree.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
bigint.h:	$(SRC)/bigint.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
dword.h:	$(SRC)/dword.h ;	 ( $(RM) $@ ; $(LN) $? $@ )
util.h: 	$(SRC)/util.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
table.h: 	$(SRC)/table.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )
xfloat.h: 	$(SRC)/xfloat.h ; 	 ( $(RM) $@ ; $(LN) $? $@ )

stdc.c: 	$(SRC)/stdc.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
cport.c:	$(SRC)/cport.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
opsys.c:	$(SRC)/opsys.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
btree.c:	$(SRC)/btree.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
bigint.c:	$(SRC)/bigint.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
dword.c:	$(SRC)/dword.c ;	 ( $(RM) $@ ; $(LN) $? $@ )
util.c: 	$(SRC)/util.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
table.c: 	$(SRC)/table.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )
xfloat.c: 	$(SRC)/xfloat.c ; 	 ( $(RM) $@ ; $(LN) $? $@ )

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
	  echo "$(CCPROG) $(CFLAGS) -c $$SRC/stdc.c" ; \
	  $(CCPROG) $(CFLAGS) -c $$SRC/stdc.c ;  \
	  $(AR) $(LIBFOAM) stdc.$(OBJ) ; \
	  $(RM) stdc.$(OBJ))

$(LIBFOAM)(cport.$(OBJ)):  cport.c  $(PORTH0) $(STDCH0)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c $$SRC/cport.c" ; \
	  $(CCPROG) $(CFLAGS) -c $$SRC/cport.c ;  \
	  $(AR) $(LIBFOAM) cport.$(OBJ) ; \
	  $(RM) cport.$(OBJ))

$(LIBFOAM)(opsys.$(OBJ)):  opsys.c  $(PORTH0) $(STDCH0) $(OPSYSH0)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c  $$SRC/opsys.c" ; \
	  $(CCPROG) $(CFLAGS) -c $$SRC/opsys.c ;  \
	  $(AR) $(LIBFOAM) opsys.$(OBJ) ; \
	  $(RM) opsys.$(OBJ))

$(LIBFOAM)(btree.$(OBJ)):  btree.c  $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c  $$SRC/btree.c" ; \
	  $(CCPROG) $(CFLAGS) -c  -I$$SRC/$(CmmDir) $$SRC/btree.c ;  \
	  $(AR) $(LIBFOAM) btree.$(OBJ) ; \
	  $(RM) btree.$(OBJ))

store $(LIBFOAM)(store.$(OBJ)):  store.c  $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c $$SRC/store.c" ; \
	  $(CCPROG) $(CFLAGS) -c  -I$$SRC/$(CmmDir)  $$SRC/store.c ;  \
	  $(AR) $(LIBFOAM) store.$(OBJ) ; \
	  $(RM) store.$(OBJ))

$(LIBFOAM)(bigint.$(OBJ)): bigint.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c  $$SRC/bigint.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir) $$SRC/bigint.c ;  \
	  $(AR) $(LIBFOAM) bigint.$(OBJ) ; \
	  $(RM) bigint.$(OBJ))

$(LIBFOAM)(dword.$(OBJ)): dword.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c $$SRC/dword.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/dword.c ;  \
	  $(AR) $(LIBFOAM) dword.$(OBJ) ; \
	  $(RM) dword.$(OBJ))

$(LIBFOAM)(util.$(OBJ)):   util.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c $$SRC/util.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/util.c ;  \
	  $(AR) $(LIBFOAM) util.$(OBJ) ; \
	  $(RM) util.$(OBJ))

$(LIBFOAM)(table.$(OBJ)):   table.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/table.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/table.c ;  \
	  $(AR) $(LIBFOAM) table.$(OBJ) ; \
	  $(RM) table.$(OBJ))

$(LIBFOAM)(xfloat.$(OBJ)):   xfloat.c   $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/xfloat.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/xfloat.c ;  \
	  $(AR) $(LIBFOAM) xfloat.$(OBJ) ; \
	  $(RM) xfloat.$(OBJ))

foam_c $(LIBFOAM)(foam_c.$(OBJ)): foam_c.c $(FOAMH)
	@(SRC=`pwd` ; FINC=`pwd` ; \
	  cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/foam_c.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/foam_c.c ;  \
	  $(AR) $(LIBFOAM) foam_c.$(OBJ) ; \
	  $(RM) foam_c.$(OBJ))

foam_cfp $(LIBFOAM)(foam_cfp.$(OBJ)): foam_cfp.c $(FOAMH)
	@(SRC=`pwd` ; FINC=`pwd` ; \
	  cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c  -I$$SRC/$(CmmDir)  $$SRC/foam_cfp.c" ; \
	  $(CCPROG) $(CFLAGS) -c  -I$$SRC/$(CmmDir)  $$SRC/foam_cfp.c ;  \
	  $(AR) $(LIBFOAM) foam_cfp.$(OBJ) ; \
	  $(RM) foam_cfp.$(OBJ))

output $(LIBFOAM)(output.$(OBJ)): output.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/output.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/output.c ;  \
	  $(AR) $(LIBFOAM) output.$(OBJ) ; \
	  $(RM) output.$(OBJ))

foamopt $(LIBFOAM)(foamopt.$(OBJ)): foamopt.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/foamopt.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir) $$SRC/foamopt.c ;  \
	  $(AR) $(LIBFOAM) foamopt.$(OBJ) ; \
	  $(RM) foamopt.$(OBJ))

compopt $(LIBFOAM)(compopt.$(OBJ)): compopt.c $(FOAMH)
	@(SRC=`pwd` ; cd $(LibDir) ; \
	  echo "$(CCPROG) $(CFLAGS) -c  $$SRC/compopt.c" ; \
	  $(CCPROG) $(CFLAGS) -c -I$$SRC/$(CmmDir)  $$SRC/compopt.c ;  \
	  $(AR) $(LIBFOAM) compopt.$(OBJ) ; \
	  $(RM) compopt.$(OBJ))
