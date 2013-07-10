THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaldor_HEADERS =	\
	aldor.as	\
	aldorinterp.as	\
	aldorio.as	\
	aldortest.as

libaldor_HEADERS := $(addprefix build/include/, $(libaldor_HEADERS))

# Copy includes
build/include/aldor.as: $(THIS)aldor.as
	mkdir -p $(dir $@)
	cp $< $@

build/include/aldorinterp.as: $(THIS)aldorinterp.as
	mkdir -p $(dir $@)
	cp $< $@

build/include/aldorio.as: $(THIS)aldorio.as
	mkdir -p $(dir $@)
	cp $< $@

build/include/aldortest.as: $(THIS)aldortest.as
	mkdir -p $(dir $@)
	cp $< $@
