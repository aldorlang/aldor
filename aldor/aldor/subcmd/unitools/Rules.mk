THIS := $(dir $(lastword $(MAKEFILE_LIST)))

unicl_SOURCES =	\
	../../src/btree.c	\
	../../src/buffer.c	\
	../../src/cfgfile.c	\
	../../src/debug.c	\
	../../src/file.c	\
	../../src/fname.c	\
	../../src/format.c	\
	../../src/list.c	\
	../../src/opsys.c	\
	../../src/ostream.c	\
	../../src/stdc.c	\
	../../src/store.c	\
	../../src/strops.c	\
	../../src/timer.c	\
	../../src/util.c	\
	../../src/xfloat.c	\
	unicl.c	\

unicl_OBJECTS := $(addprefix $(THIS), $(unicl_SOURCES:.c=$(OBJEXT)))

$(BINDIR)/unicl$(EXEEXT): $(unicl_OBJECTS)
	mkdir -p $(dir $@)
	$(LINK.c) $+ -o $@


$(THIS)%$(OBJEXT): $(THIS)%.c
	$(COMPILE.c) $< -o $@ -I$(dir $@)../../src


clean: clean-unicl
clean-unicl:
	$(RM) $(unicl_OBJECTS)
	$(RM) $(unicl_BUILT_SOURCES)
	$(RM) $(BINDIR)/unicl$(EXEEXT)
