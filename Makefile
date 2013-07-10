export ALDORROOT := $(CURDIR)/build
export PATH := $(ALDORROOT):$(PATH)

AFLAGS =			\
	-I build/include	\
	-Y build		\
	-Y $(dir $@)		\
	-Q8 -Qinline-all	\
	-fao=$(@:.o=.ao)	\
	-fo

TARGETS =		\
	build/aldor	\
	build/libruntime.a	\
	build/libaldor.a	\
	build/libalgebra.a	\
	build/libaxllib.a	\
	build/libaxldem.a	\
	build/libfoam.a	\
	build/libfoamlib.a

all: $(TARGETS)

RLWRAP = $(shell which rlwrap)

loop:
	$(RLWRAP) build/aldor -Y build -I build/include -Gloop

include aldor/Rules.mk
