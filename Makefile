export ALDORROOT := $(CURDIR)/build

AFLAGS =			\
	-I build/include	\
	-Y build		\
	-Y $(dir $@)		\
	-Q8 -Qinline-all	\
	-fao=$(@:.c=.ao)	\
	-fc=$@

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

include aldor/Rules.mk
