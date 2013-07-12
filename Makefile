# Parallel build doesn't work, yet.
.NOTPARALLEL:

# Toolchain selection.
include config.mk

# Find lex/flex.
LEX = $(shell which flex)
ifeq ($(LEX),)
LEX = $(shell which lex)
endif
ifeq ($(LEX),)
$(error lex or flex are not installed; cannot build aldor)
endif

# Find bison/yacc.
YACC = $(shell which bison)
ifeq ($(YACC),)
YACC = $(shell which byacc)
endif
ifeq ($(YACC),)
YACC = $(shell which yacc)
endif
ifeq ($(LEX),)
$(error bison, byacc or yacc are not installed; cannot build aldor)
endif

export ALDORROOT := build

BINDIR = $(ALDORROOT)/bin
INCDIR = $(ALDORROOT)/include
LIBDIR = $(ALDORROOT)/lib

export PATH := $(BINDIR):$(PATH)

AFLAGS =			\
	-I $(INCDIR)		\
	-Y $(LIBDIR)		\
	-Y $(dir $@)		\
	-Q8 -Qinline-all	\
	-fao=$(@:.o=.ao)	\
	-fo=$@

TARGETS =		\
	$(BINDIR)/aldor	\
	$(LIBDIR)/libaldor.a	\
	$(LIBDIR)/libalgebra.a	\
	$(LIBDIR)/libaxllib.a	\
	$(LIBDIR)/libaxldem.a	\
	$(LIBDIR)/libfoam.a	\
	$(LIBDIR)/libfoamlib.a	\
	$(LIBDIR)/aldor_gloop.ao

all: $(TARGETS)

RLWRAP = $(shell which rlwrap)

loop:
	$(RLWRAP) $(BINDIR)/aldor -Y $(LIBDIR) -I $(INCDIR) -Gloop

include aldor/Rules.mk
