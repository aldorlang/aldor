THIS := $(dir $(lastword $(MAKEFILE_LIST)))

# Copy includes
$(INCDIR)/algebra.as: $(THIS)algebra.as
	cp $< $@

$(INCDIR)/algebrauid.as: $(THIS)algebrauid.as
	cp $< $@
