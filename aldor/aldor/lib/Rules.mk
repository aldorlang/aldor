THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)libfoam/Rules.mk $(THIS)libfoamlib/Rules.mk
