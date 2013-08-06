SUBDIRS =	\
	bug1332	\
	bug1333	\
	bug1334	\
	bug1337	\
	bug1340	\
	intfact	\
	issue2	\
	issue38

TESTS =			\
	bug1332/bug1332	\
	bug1333/bug1333	\
	bug1334/bug1334	\
	bug1337/bug1337	\
	bug1340/bug1340	\
	intfact/intfact	\
	issue2/issue2	\
	issue38/issue38

XFAIL_TESTS =		\
	issue2/issue2

check: $(TESTS:=.out)
	@for T in $(TESTS); do					\
	  if ! diff $$T.out $(srcdir)/$$T.ref > $$T.diff; then	\
	    echo "DIFF: $$T";					\
	  fi;							\
	done

# Cut off first three lines.
%.out: %.log
	tail -n`expr \`wc -l < $<\` - 3` $< > $@
