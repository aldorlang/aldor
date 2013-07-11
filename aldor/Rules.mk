THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)aldor/Rules.mk $(THIS)lib/Rules.mk
