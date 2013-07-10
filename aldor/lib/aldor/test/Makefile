#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

execs = sal_cmdline.test sal_random.test sal_stream.test sal_intgmp.test
opts = -fx -y$(ALDORLIBROOT)/lib
options = -q5 -qinline-all -laldor
# options = -q1 -Zdb -dDEBUG -laldord
# options = -q1 -dDEBUG -laldord

all: $(execs)
	./testall

.DEFAULT:
	aldor $(options) $(opts) $<.as

sal_intgmp.test:
	aldor -cruntime=foam-gmp,gmp $(options) $(opts) sal_intgmp.test.as

.PHONY: clean
clean:
	-rm -f $(execs) stamp-*

fullclean: clean
	-rm -f sal_*.test.as ald_*.test.as rtexns.test.as
