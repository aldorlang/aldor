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
	../../src/stdc.c	\
	../../src/store.c	\
	../../src/strops.c	\
	../../src/timer.c	\
	../../src/util.c	\
	../../src/xfloat.c	\
	unicl.c	\

unicl_OBJECTS := $(addprefix $(THIS), $(unicl_SOURCES:.c=.o))

build/unicl: $(unicl_OBJECTS)
	$(LINK.c) $+ -o $@


$(THIS)%.o: $(THIS)%.c
	$(COMPILE.c) $< -o $@ -I$(dir $@)../../src


clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(unicl_OBJECTS)
	$(RM) $(unicl_BUILT_SOURCES)
	$(RM) build/unicl
