THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)aldor/Rules.mk $(THIS)algebra/Rules.mk $(THIS)axllib/Rules.mk $(THIS)libaxldem/Rules.mk
