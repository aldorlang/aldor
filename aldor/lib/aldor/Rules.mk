THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)include/Rules.mk $(THIS)src/Rules.mk
