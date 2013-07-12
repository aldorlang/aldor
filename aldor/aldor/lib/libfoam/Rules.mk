THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libfoam_ASOURCES :=	\
	basictuple.as	\
	dispatchvector.as	\
	domain.as	\
	stringtable.as	\
	dd.as	\
	dv.as	\
	aldorcatrep.as	\
	aldordomainrep.as	\
	box.as	\
	catdispatchvector.as	\
	catobj.as	\
	lazyimport.as	\
	pointerdomain.as	\
	ptrcache.as	\
	ptrcatobj.as	\
	runtime.as

libfoam_ASOURCES := $(addprefix $(THIS), $(libfoam_ASOURCES))

libfoam_AOBJECTS := $(libfoam_ASOURCES:.as=.ao)
libfoam_COBJECTS := $(libfoam_ASOURCES:.as=$(OBJEXT))


# C library
$(LIBDIR)/libfoam$(LIBEXT): $(libfoam_COBJECTS) $(libruntime_OBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%$(OBJEXT): $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS)
	$(AR) cr libfoam_$*.al $(filter %.ao, $(^:$(OBJEXT)=.ao))
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) -Q9 -LRuntimeLib=libfoam_$*.al $<
	$(RM) libfoam_$*.al
	$(AR) cr $(LIBDIR)/libfoam.al $(@:$(OBJEXT)=.ao)

# Specialised build rule for runtime
$(THIS)runtime$(OBJEXT): $(THIS)runtime.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS)
	$(AR) cr libfoam_runtime.al $(filter %.ao, $(^:$(OBJEXT)=.ao))
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) -Q9 -Wruntime -LRuntimeLib=libfoam_runtime.al $<
	$(RM) libfoam_runtime.al
	# Delete everything from libfoam and make one with just runtime.ao in it
	$(RM) $(LIBDIR)/libfoam.al
	$(AR) cr $(LIBDIR)/libfoam.al $(@:$(OBJEXT)=.ao)

# Clean
clean: clean-libfoam
clean-libfoam:
	$(RM) $(libfoam_AOBJECTS)
	$(RM) $(libfoam_COBJECTS)
	$(RM) $(libfoam_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libfoam$(LIBEXT) $(LIBDIR)/libfoam.al

# Depend
$(THIS)basictuple$(OBJEXT):	\
	$(LIBDIR)/libfoamlib$(LIBEXT)

$(THIS)aldorcatrep$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)catdispatchvector$(OBJEXT)	\
	$(THIS)catobj$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)	\
	$(THIS)ptrcache$(OBJEXT)	\
	$(THIS)stringtable$(OBJEXT)
$(THIS)aldordomainrep$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)catdispatchvector$(OBJEXT)	\
	$(THIS)catobj$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)	\
	$(THIS)ptrcache$(OBJEXT)	\
	$(THIS)stringtable$(OBJEXT)
$(THIS)catdispatchvector$(OBJEXT):	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)
$(THIS)catobj$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)catdispatchvector$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)	\
	$(THIS)ptrcache$(OBJEXT)
$(THIS)dd$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)dv$(OBJEXT)	\
	$(THIS)stringtable$(OBJEXT)
$(THIS)dispatchvector$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)
$(THIS)domain$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)box$(OBJEXT)
$(THIS)dv$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)
$(THIS)lazyimport$(OBJEXT):	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)
$(THIS)pointerdomain$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)
$(THIS)ptrcache$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)
$(THIS)ptrcatobj$(OBJEXT):	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)catdispatchvector$(OBJEXT)	\
	$(THIS)catobj$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)	\
	$(THIS)ptrcache$(OBJEXT)
$(THIS)runtime$(OBJEXT):	\
	$(THIS)aldorcatrep$(OBJEXT)	\
	$(THIS)aldordomainrep$(OBJEXT)	\
	$(THIS)basictuple$(OBJEXT)	\
	$(THIS)box$(OBJEXT)	\
	$(THIS)catdispatchvector$(OBJEXT)	\
	$(THIS)catobj$(OBJEXT)	\
	$(THIS)dispatchvector$(OBJEXT)	\
	$(THIS)domain$(OBJEXT)	\
	$(THIS)lazyimport$(OBJEXT)	\
	$(THIS)pointerdomain$(OBJEXT)	\
	$(THIS)ptrcache$(OBJEXT)	\
	$(THIS)ptrcatobj$(OBJEXT)	\
	$(THIS)stringtable$(OBJEXT)
