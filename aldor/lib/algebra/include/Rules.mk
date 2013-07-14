THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libalgebra_HEADERS =	\
	algebra.as	\
	algebrauid.as

# Copy includes
$(INCDIR)/algebra.as: $(THIS)algebra.as
	cp $< $@

$(INCDIR)/algebrauid.as: $(THIS)algebrauid.as
	cp $< $@
