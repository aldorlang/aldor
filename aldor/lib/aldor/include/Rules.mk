THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaldor_HEADERS =	\
	aldor.as	\
	aldorinterp.as	\
	aldorio.as	\
	aldortest.as

libaldor_HEADERS := $(addprefix $(INCDIR)/, $(libaldor_HEADERS))

# Copy includes
$(INCDIR)/aldor.as: $(THIS)aldor.as
	cp $< $@

$(INCDIR)/aldorinterp.as: $(THIS)aldorinterp.as
	cp $< $@

$(INCDIR)/aldorio.as: $(THIS)aldorio.as
	cp $< $@

$(INCDIR)/aldortest.as: $(THIS)aldortest.as
	cp $< $@
