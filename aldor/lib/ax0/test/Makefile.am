libraries = ax0

check_DATA =	\
	axlist0.ao	\
	coerce1.ao	\
	coerce2.ao	\
	coerce3.ao	\
	coerce4.ao	\
	coerce5.ao	\
	convert0.ao	\
	intcat0.ao	\
	intcat1.ao	\
	segext5.ao	\
	segext6.ao	\
	segext7.ao	\
	t619.ao	\
	vector0.ao	\
	vector1.ao	\
	vector2.ao	\
	#

BROKEN =		\
	hilbert0.ao	\
	matops.ao	\
	supcat2.ao	\
	supcat3.ao	\
	#

TESTS = $(check_DATA)

XFAIL_TESTS =	\
	coerce1.ao	\
	coerce2.ao	\
	coerce3.ao	\
	coerce4.ao	\
	intcat1.ao	\
	vector0.ao	\
	#


aldorsrcdir	:= $(abs_top_srcdir)/aldor/src
aldorexedir	:= $(abs_top_builddir)/aldor/src
aldorlibdir	:= $(abs_top_builddir)/aldor/lib

foamlibdir	:= $(abs_top_builddir)/aldor/lib/libfoam

libraryincdirs	:= $(foreach L,$(libraries),$(abs_top_srcdir)/lib/$L/include)
librarylibdirs	:= $(foreach L,$(libraries),$(abs_top_builddir)/lib/$L/src)

ALDOR		:= $(aldorexedir)/aldor$(EXEEXT)
ALDORFLAGS	:=				\
	-Nfile=$(aldorsrcdir)/aldor.conf	\
	$(addprefix -I,$(libraryincdirs))	\
	$(addprefix -Y,$(librarylibdirs))	\
	$(ALFLAGS)

# Aldor silent-rules
AM_V_ALDOR = $(am__v_ALDOR_$(V))
am__v_ALDOR_ = $(am__v_ALDOR_$(AM_DEFAULT_VERBOSITY))
am__v_ALDOR_0 = @echo "  ALDOR " $@;

%.ao: %.as $(ALDOR)
	@$(MKDIR_P) $(@D)
	$(AM_V_ALDOR)$(ALDOR) $(ALDORFLAGS) -Y$(@D) -Fao=$@ $<


TEST_EXTENSIONS = .ao
AO_LOG_COMPILER = $(ALDOR) $(ALDORFLAGS) -Y$(foamlibdir)/al -lax0 -ginterp
