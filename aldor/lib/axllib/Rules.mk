THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)src/Rules.mk
