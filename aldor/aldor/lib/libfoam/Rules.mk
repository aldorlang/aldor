THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libfoam_ASOURCES :=	\
	runtime.as

libfoam_ASOURCES := $(addprefix $(THIS), $(libfoam_ASOURCES))

libfoam_AOBJECTS := $(libfoam_ASOURCES:.as=.ao)
libfoam_COBJECTS := $(libfoam_ASOURCES:.as=.o)


# C library
build/libfoam.a: $(libfoam_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as build/aldor build/unicl build/include/aldor.conf
	build/aldor $(AFLAGS) -Q3 -Wruntime $<
	$(AR) cr build/libfoam.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libfoam_AOBJECTS)
	$(RM) $(libfoam_COBJECTS)
	$(RM) $(libfoam_ASOURCES:.as=.c)
	$(RM) build/libfoam.a build/libfoam.al

# Depend
$(THIS)runtime.o:	build/libfoamlib.a
