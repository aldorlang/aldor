THIS := $(dir $(lastword $(MAKEFILE_LIST)))

# Copy includes
build/include/aldor.as: $(THIS)aldor.as
	cp $< $@

build/include/aldorinterp.as: $(THIS)aldorinterp.as
	cp $< $@

build/include/aldorio.as: $(THIS)aldorio.as
	cp $< $@

build/include/aldortest.as: $(THIS)aldortest.as
	cp $< $@
