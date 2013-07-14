# Copyright © 2007 Pippijn van Steenhoven

.SILENT:
# TODO: clean this up

default: test

PERL		= /usr/bin/perl
PERLFLAGS	= `${PERL} -MExtUtils::Embed -e ccopts` -Wno-write-strings -Wno-redundant-decls -Wno-unused
PERLPRIVLIBEXP	= `${PERL} -MConfig -e 'print $$Config{privlibexp}'`
PERLLIB		= `${PERL} -MExtUtils::Embed -e ldopts`

OPTS =			\
	-O3		\
	-ggdb3		\
	-std=c++98	\
	-pipe		\

WARNINGS =			\
	-Wall			\
	-Wpointer-arith		\
	-Wimplicit		\
	-Wcast-align		\
	-Wcast-qual		\
	-Wextra			\
	-W			\
	-Wno-unused-parameter	\
	-Wshadow		\
	-Wundef			\
	-Wpacked		\
	-Wunused-function	\
	-Wunused-label		\
	-Wunused-value		\
	-Wunused-variable	\
	-Wredundant-decls	\
	-Wfloat-equal		\
	-Wformat=1		\
	-Wdisabled-optimization	\
	-Wconversion		\
	-Wwrite-strings		\
	-Wno-variadic-macros	\
	-Wnon-virtual-dtor	\
	-fmessage-length=0	\
	-Werror			\
	-ansi			\
	-pedantic		\
	-pedantic-errors	\
	-std=c++98		\
	#-Wfatal-errors		\

CXXFLAGS =			\
	-O3			\
	-std=c++98		\
	-fstrict-aliasing	\
	-fstack-protector-all	\
	-fstack-check		\

all: eval/bin/evaluator eval/bin/showlog eval/bin/Evaluator.so eval/bin/Evaluator.pm

test: all
	sudo perl eval/bin/evaluator

trace: all
	sudo strace -f perl eval/bin/evaluator

upload: all
	sh perms.rt
	rsync -avP eval/ ra:public_html/eval

memcheck: all
	sudo valgrind --num-callers=20 --leak-check=full --leak-resolution=high perl evaluator

src/syscalls$(ARCH).C: src/getsyscalls
	sh $< > $@

eval/bin/Evaluator.so: Evaluator.o syscalls$(ARCH).o
	echo "  LD  $@"
	${CXX} -shared ${CXXFLAGS} $+ -o $@ ${PERLLIB}

eval/bin/evaluator: src/evaluator
	cp $< $@

eval/bin/showlog: src/showlog
	cp $< $@

eval/bin/Evaluator.pm: src/Evaluator.pm
	cp $< $@

%.C: %.xs src/typemap
	echo "Generating $@"; \
	   PERL="${PERL}" \
	   ${PERL} ${PERLPRIVLIBEXP}/ExtUtils/xsubpp -C++ -csuffix .C \
	   -typemap ${PERLPRIVLIBEXP}/ExtUtils/typemap \
	   -typemap typemap \
	   -prototypes $< > $@;

%.o: src/%.C
	echo "  CXX  $@"
	${CXX} -fPIC -c ${OPTS} ${CXXFLAGS} ${PERLFLAGS} $< -o $@

# Extra dependencies
Evaluator.o: src/utf.h

clean:
	rm -f *.o src/Evaluator.C eval/bin/Evaluator.so

distclean:
	rm -f eval/bin/* .depend

rt:
	if [ -z "$(ALDORROOT)" ]; then					\
	  echo 'Please set $$ALDORROOT before running "make rt"';	\
	  false;							\
	fi
	mkdir -p eval/rt64/bin
	mkdir -p eval/rt64/include
	mkdir -p eval/rt64/lib/x86_64-linux-gnu
	mkdir -p eval/rt64/lib64
	echo "Installing compiler and libraries"
	cp $(ALDORROOT)/bin/aldor eval/rt64/bin/
	cp $(ALDORROOT)/include/*.as eval/rt64/include/
	cp $(ALDORROOT)/include/aldor.conf eval/rt64/include/
	cp $(ALDORROOT)/include/sample.terminfo eval/rt64/include/
	cp $(ALDORROOT)/lib/*.a[lo] eval/rt64/lib/
	echo "Installing minimal Linux environment"
	cp /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 eval/rt64/lib/x86_64-linux-gnu/
	cp /lib/x86_64-linux-gnu/libc.so.6 eval/rt64/lib/x86_64-linux-gnu/
	ln -sf /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 eval/rt64/lib64/

Evaluator.o: src/syscalls.h
