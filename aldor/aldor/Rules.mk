THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)lib/Rules.mk $(THIS)src/Rules.mk $(THIS)tools/Rules.mk
