THIS := $(dir $(lastword $(MAKEFILE_LIST)))
include $(THIS)tools/Rules.mk $(THIS)src/Rules.mk $(THIS)subcmd/Rules.mk $(THIS)lib/Rules.mk
