#########################################################
# :: Configuration selection.
#########################################################

ifeq ($(CONFIG),)
CONFIG = unix
#CONFIG = unix32
#CONFIG = unix64
#CONFIG = ming32
#CONFIG = cygwin
endif


#########################################################
# :: Configurations.
#########################################################

# Default UNIX-like platform, let the environment decide CC and AR.
ifeq ($(CONFIG), unix)
OBJEXT = .o
LIBEXT = .a
EXEEXT =
endif


# Explicitly pass -m32 to the compiler for 32 bit compilation.
ifeq ($(CONFIG), unix32)
CFLAGS += -m32

OBJEXT = .o
LIBEXT = .a
EXEEXT =
endif


# Explicitly pass -m64 to the compiler for 64 bit compilation.
ifeq ($(CONFIG), unix64)
CFLAGS += -m64

OBJEXT = .o
LIBEXT = .a
EXEEXT =
endif


# Mingw32 toolchain.
ifeq ($(CONFIG), ming32)
CC = i686-pc-mingw32-gcc
AR = i686-pc-mingw32-ar

OBJEXT = .o
LIBEXT = .a
EXEEXT = .exe
endif


# Cygwin toolchain.
ifeq ($(CONFIG), cygwin)
CC = i686-pc-cygwin-gcc
AR = i686-pc-cygwin-ar

OBJEXT = .o
LIBEXT = .a
EXEEXT = .exe
endif



#########################################################
# :: Check whether any configuration was chosen
#########################################################

ifeq ($(OBJEXT)$(LIBEXT)$(EXEEXT),)
$(error no configuration chosen; please take a look at config.mk)
endif
