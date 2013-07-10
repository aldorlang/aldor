THIS := $(dir $(lastword $(MAKEFILE_LIST)))

# Copy includes
$(INCDIR)/algebra.as: $(THIS)algebra.as
	mkdir -p $(dir $@)
	cp $< $@

$(INCDIR)/algebrauid.as: $(THIS)algebrauid.as
	mkdir -p $(dir $@)
	cp $< $@
