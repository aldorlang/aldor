#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

execs = sit_extree.test sit_dup.test sit_sqfree.test sit_spf.test\
	sit_vector.test sit_dnsemat.test sit_qotient.test sit_zpf.test\
	sit_umonom.test sit_zfactor.test sit_chrem.test\
	alg_sup0.test alg_sup1.test sit_modgcd.test\
	sm_dmp0.test sm_expocat.test alg_defgcd.test alg_modgcdp.test

opts = -fx -y$(ALDORLIBROOT)/lib -y$(ALGEBRAROOT)/lib

# For the release version of Algebra:
options = -lalgebra -laldor -q1 -qinline-all

# For the gmp version of Algebra:
# options = -lalgebra-gmp -laldor -q1 -qinline-all -cruntime=foam-gmp,gmp -dGMP

# For the debug version of Algebra:
# options = -lalgebrad -laldord -q1 -dDEBUG

all: $(execs)

.DEFAULT:
	$(ALDORROOT)/bin/aldor $(options) $(opts) $<.as

.PHONY: clean
clean:
	-rm $(execs)

.PHONY: cleanall
cleanall:
	-rm $(execs) sit_*.test.as stamp-*

