##########################################
#
# Makefile to build an Aldor distribution
#
# Yannis Chicha, July 2003
#
##########################################

all:
	@echo "Usage:"
	@echo "   make generic      => make the generic, platform-independent version of the libs"
	@echo "   make cleansources => invoke make clean in each lib directory"

# ----------------------------------------------------

generic:
	@(echo "LIBS MAKE GENERIC"; \
	  PATH="$(ALDORROOT)/bin:$(PATH)"; \
	  export PATH; \
	  export GENERIC_DIR; \
	  for i in $(LIBS_TO_BUILD); do cd $$i; $(MAKE) generic; cd ..; done)

cleansources:
	@for i in $(LIBS_TO_BUILD); do echo "Cleaning sources for $$i"; cd $$i; $(MAKE) clean; cd ..; done
