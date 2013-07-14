THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaldor_HEADERS =	\
	aldor.as	\
	aldorinterp.as	\
	aldorio.as	\
	aldortest.as

# Copy includes
$(INCDIR)/aldor.as: $(THIS)aldor.as
	cp $< $@

$(INCDIR)/aldorinterp.as: $(THIS)aldorinterp.as
	cp $< $@

$(INCDIR)/aldorio.as: $(THIS)aldorio.as
	cp $< $@

$(INCDIR)/aldortest.as: $(THIS)aldortest.as
	cp $< $@
