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
libfoam_COBJECTS := $(libfoam_ASOURCES:.as=.o)


# C library
$(LIBDIR)/libfoam.a: $(libfoam_COBJECTS) $(libruntime_OBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as $(BINDIR)/aldor $(BINDIR)/unicl $(aldor_HEADERS)
	$(AR) cr libfoam_$*.al $(filter %.ao, $(^:.o=.ao))
	$(BINDIR)/aldor $(AFLAGS) -Q9 -LRuntimeLib=libfoam_$*.al $<
	$(RM) libfoam_$*.al
	$(AR) cr $(LIBDIR)/libfoam.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Clean
clean: clean-libfoam
clean-libfoam:
	$(RM) $(libfoam_AOBJECTS)
	$(RM) $(libfoam_COBJECTS)
	$(RM) $(libfoam_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libfoam.a $(LIBDIR)/libfoam.al

# Depend
$(THIS)basictuple.o:	\
	$(LIBDIR)/libfoamlib.a

$(THIS)aldorcatrep.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)catdispatchvector.o	\
	$(THIS)catobj.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o	\
	$(THIS)ptrcache.o	\
	$(THIS)stringtable.o
$(THIS)aldordomainrep.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)catdispatchvector.o	\
	$(THIS)catobj.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o	\
	$(THIS)ptrcache.o	\
	$(THIS)stringtable.o
$(THIS)catdispatchvector.o:	\
	$(THIS)box.o	\
	$(THIS)domain.o
$(THIS)catobj.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)catdispatchvector.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o	\
	$(THIS)ptrcache.o
$(THIS)dd.o:	\
	$(THIS)basictuple.o	\
	$(THIS)dv.o	\
	$(THIS)stringtable.o
$(THIS)dispatchvector.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o
$(THIS)domain.o:	\
	$(THIS)basictuple.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)box.o
$(THIS)dv.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o
$(THIS)lazyimport.o:	\
	$(THIS)box.o	\
	$(THIS)domain.o
$(THIS)pointerdomain.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o
$(THIS)ptrcache.o:	\
	$(THIS)basictuple.o
$(THIS)ptrcatobj.o:	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)catdispatchvector.o	\
	$(THIS)catobj.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o	\
	$(THIS)ptrcache.o
$(THIS)runtime.o:	\
	$(THIS)aldorcatrep.o	\
	$(THIS)aldordomainrep.o	\
	$(THIS)basictuple.o	\
	$(THIS)box.o	\
	$(THIS)catdispatchvector.o	\
	$(THIS)catobj.o	\
	$(THIS)dispatchvector.o	\
	$(THIS)domain.o	\
	$(THIS)lazyimport.o	\
	$(THIS)pointerdomain.o	\
	$(THIS)ptrcache.o	\
	$(THIS)ptrcatobj.o	\
	$(THIS)stringtable.o
