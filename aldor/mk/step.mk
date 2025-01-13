# Makefile.in will need _AM_DEFAULT_VERBOSITY defined
#  AM_DEFAULT_VERBOSITY = @AM_DEFAULT_VERBOSITY@

$(if $(AM_DEFAULT_VERBOSITY),,$(error "oops - AM_DEFAULT_VERBOSITY should be defined in source makefile"))

# Echo step name and target (v and p versions - v takes the name from $* or $*, and p takes it from a parameter)
define am_auto_template
AM_V_$(1)    = $$(am__v_$(1)_$$(V))
am__v_$(1)_  = $$(am__v_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__v_$(1)_0 = @echo "  $(subst _,-,$(1))   " $$(if $$*,$$*,$$@);
am__v_$(1)_1 =
am__v_$(1)_2 = @echo "  $(subst _,-,$(1))   " $$(if $$*,$$*,$$@); set -x;
AM_P_$(1)    = $$(am__p_$(1)_$$(V))
am__p_$(1)_  = $$(am__p_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__p_$(1)_0 = @echo "  $(subst _,-,$(1))   " $$(1);
am__p_$(1)_1 =
endef

# Silence unless told otherwise
define am_auto_template_quiet
AM_V_$(1)    = $$(am__v_$(1)_$$(V))
am__v_$(1)_  = $$(am__v_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__v_$(1)_0 = @
endef

define am_define_steps
$(foreach rule,$(1), \
	  $(eval $(call am_auto_template,$(rule))))
endef

define am_define_steps_quiet
$(foreach rule,$(1), \
	  $(eval $(call am_auto_template_quiet,$(rule))))
endef
