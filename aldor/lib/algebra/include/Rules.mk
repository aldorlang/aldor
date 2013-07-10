THIS := $(dir $(lastword $(MAKEFILE_LIST)))

# Copy includes
build/include/algebra.as: $(THIS)algebra.as
	mkdir -p $(dir $@)
	cp $< $@

build/include/algebrauid.as: $(THIS)algebrauid.as
	mkdir -p $(dir $@)
	cp $< $@
