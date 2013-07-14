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

# Generate all C sources
libfoam-sources: $(libfoam_ASOURCES:.as=.c)

# C library
$(LIBDIR)/libfoam$(LIBEXT): $(libfoam_COBJECTS) $(libruntime_OBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.ao: $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS)
	$(AR) cr libfoam_$*.al $(filter %.ao, $^)
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) -Q9 -LRuntimeLib=libfoam_$*.al $<
	$(RM) libfoam_$*.al
	$(AR) cr $(LIBDIR)/libfoam.al $@

# Specialised build rule for runtime
$(THIS)runtime.ao: $(THIS)runtime.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS)
	$(AR) cr libfoam_runtime.al $(filter %.ao, $^)
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) -Q9 -Wruntime -LRuntimeLib=libfoam_runtime.al $<
	$(RM) libfoam_runtime.al
	# Delete everything from libfoam and make one with just runtime.ao in it
	$(RM) $(LIBDIR)/libfoam.al
	$(AR) cr $(LIBDIR)/libfoam.al $@

# Clean
clean: libfoam-clean
libfoam-clean:
	$(RM) $(libfoam_AOBJECTS)
	$(RM) $(libfoam_COBJECTS)
	$(RM) $(libfoam_ASOURCES:.as=.c)
	$(RM) $(libfoam_ASOURCES:.as=.fm)
	$(RM) $(LIBDIR)/libfoam$(LIBEXT) $(LIBDIR)/libfoam.al

# Depend
$(THIS)basictuple.ao:	\
	$(LIBDIR)/libfoamlib$(LIBEXT)

$(THIS)aldorcatrep.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)catdispatchvector.ao	\
	$(THIS)catobj.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao	\
	$(THIS)ptrcache.ao	\
	$(THIS)stringtable.ao
$(THIS)aldordomainrep.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)catdispatchvector.ao	\
	$(THIS)catobj.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao	\
	$(THIS)ptrcache.ao	\
	$(THIS)stringtable.ao
$(THIS)catdispatchvector.ao:	\
	$(THIS)box.ao	\
	$(THIS)domain.ao
$(THIS)catobj.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)catdispatchvector.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao	\
	$(THIS)ptrcache.ao
$(THIS)dd.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)dv.ao	\
	$(THIS)stringtable.ao
$(THIS)dispatchvector.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao
$(THIS)domain.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)box.ao
$(THIS)dv.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao
$(THIS)lazyimport.ao:	\
	$(THIS)box.ao	\
	$(THIS)domain.ao
$(THIS)pointerdomain.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao
$(THIS)ptrcache.ao:	\
	$(THIS)basictuple.ao
$(THIS)ptrcatobj.ao:	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)catdispatchvector.ao	\
	$(THIS)catobj.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao	\
	$(THIS)ptrcache.ao
$(THIS)runtime.ao:	\
	$(THIS)aldorcatrep.ao	\
	$(THIS)aldordomainrep.ao	\
	$(THIS)basictuple.ao	\
	$(THIS)box.ao	\
	$(THIS)catdispatchvector.ao	\
	$(THIS)catobj.ao	\
	$(THIS)dispatchvector.ao	\
	$(THIS)domain.ao	\
	$(THIS)lazyimport.ao	\
	$(THIS)pointerdomain.ao	\
	$(THIS)ptrcache.ao	\
	$(THIS)ptrcatobj.ao	\
	$(THIS)stringtable.ao
