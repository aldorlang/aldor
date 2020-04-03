
define am_auto_template
$(warning step $(1))
AM_V_$(1)    = $$(am__v_$(1)_$$(V))
am__v_$(1)_  = $$(am__v_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__v_$(1)_0 = @echo "  $(subst _,-,$(1))   " $$@;
endef

steps = $(foreach step,$(1),$(eval $(call am_auto_template,$(step))))
