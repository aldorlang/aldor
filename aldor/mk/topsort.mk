define rec_dep_template
$(foreach l, $($1_deps),$(call rec_dep_template,$(l)) $(l))
endef

uniq_0 = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))
uniq = $(call uniq_0,$1)
topsort_one = $(call uniq,call rec_dep_template,$1))
topsort_list = $(call uniq,$(foreach x,$1,$(call rec_dep_template,$(x)) $(x)))

