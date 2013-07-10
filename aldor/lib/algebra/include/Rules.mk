THIS := $(dir $(lastword $(MAKEFILE_LIST)))

# Copy includes
build/include/algebra.as: $(THIS)algebra.as
	cp $< $@

build/include/algebrauid.as: $(THIS)algebrauid.as
	cp $< $@
