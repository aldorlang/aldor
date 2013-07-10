##############################################################################
#
# Cross platform build system recipe. Requires cmake tool. Builds only
# compiler with its runtime library. Does not build gmp versions and it builds
# only a small set of tools.
#
# Requires ALDORROOT to be set to the place where the binaries are built.
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
##############################################################################

project(aldor)

set(libext a)
set(binext "")
set(objext o)
set(LIB "lib -nologo ")
set(RM rm)
set(MV mv)
set(CP cp)

set(ENV{ALDORROOT} "${PROJECT_BINARY_DIR}")
message("$ENV{ALDORROOT}")

IF(MSVC)
    set(CMAKE_C_FLAGS  "-DWIN32 -D_WINDOWS -W1 -Zm1000 -EHsc -GR -D_CRT_SECURE_NO_WARNINGS")
    set(libext lib)
    set(binext .exe)
    set(objext obj)
ENDIF(MSVC)


IF(CYGWIN)
    add_definitions(-mno-cygwin)
ENDIF(CYGWIN)

#flags specific to aldor compiler:
#NDEBUG
#UnBPack
#TEST_ALL
#UNUSED_LABELS
#STO_DEBUG_DISPLAY
#STO_CAN_BLACKLIST
#NLOCAL
#USE_MEMORY_CLIMATE
ADD_DEFINITIONS(-DTEST_ALL -DNDEBUG -DSTO_CAN_BLACKLIST -DSTO_DEBUG_DISPLAY)



include_directories(aldor/src)
set(srcdir aldor/src)
set(foamlibsrcdir ${CMAKE_CURRENT_SOURCE_DIR}/aldor/lib/libfoamlib)

# create directory structure
# this is built by the config system!
#file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/lib)

#
#fisrt build the tools
##############################################################################

# echon
add_executable(echon aldor/tools/port/echon.c)
install(TARGETS echon
        DESTINATION bin)

# extract
add_executable(extract aldor/tools/unix/extract.c aldor/tools/unix/flags.c)
install(TARGETS extract
        DESTINATION bin)
# msgcat
add_executable(msgcat aldor/tools/unix/msgcat.c)
install(TARGETS msgcat
        DESTINATION bin)
# testaldor
set(TestAldorDir aldor/subcmd/testaldor)
add_executable(testaldor ${TestAldorDir}/testaldor.c)
install(TARGETS testaldor
        DESTINATION bin)

#unicl
set(UniclDir aldor/subcmd/unitools)
add_executable(unicl ${UniclDir}/unicl2.c ${srcdir}/file.c ${srcdir}/fname.c ${srcdir}/opsys.c ${srcdir}/store.c ${srcdir}/memclim.c ${srcdir}/fluid.c ${srcdir}/btree.c ${srcdir}/stdc.c ${srcdir}/strops.c ${srcdir}/debug.c ${srcdir}/xfloat.c ${srcdir}/buffer.c ${srcdir}/timer.c ${srcdir}/util.c ${srcdir}/list.c ${srcdir}/format.c ${srcdir}/cfgfile.c)
install(TARGETS unicl
        DESTINATION bin)

#uniar
add_executable(uniar aldor/subcmd/unitools/uniar2.c)
install(TARGETS uniar
        DESTINATION $ENV{ALDORROOT}/bin)

#platform
add_executable(platform aldor/subcmd/unitools/platform.c)
install(TARGETS platform
        DESTINATION bin)

#
#second build the compiler
##############################################################################

#Level 4: Compiler Top Level
set(lev4 ${srcdir}/axlcomp.c ${srcdir}/cmdline.c ${srcdir}/main.c)
set(lev4_t ${srcdir}/axlcomp.c ${srcdir}/cmdline.c ${srcdir}/main_t.c)
#SOURCE_GROUP(lev4 FILES ${lev4})


# Level 3: Compiler Phases and Subordinates
set(lev3 ${srcdir}/phase.c ${srcdir}/bloop.c ${srcdir}/include.c ${srcdir}/scan.c ${srcdir}/syscmd.c ${srcdir}/linear.c
    ${srcdir}/parseby.c ${srcdir}/macex.c ${srcdir}/abnorm.c ${srcdir}/abcheck.c ${srcdir}/abuse.c ${srcdir}/scobind.c ${srcdir}/ti_decl.c
    ${srcdir}/tinfer.c ${srcdir}/ti_bup.c ${srcdir}/ti_tdn.c ${srcdir}/ti_sef.c ${srcdir}/terror.c ${srcdir}/genfoam.c ${srcdir}/gf_fortran.c
    ${srcdir}/gf_add.c ${srcdir}/gf_gener.c ${srcdir}/gf_imps.c ${srcdir}/gf_excpt.c ${srcdir}/gf_reference.c ${srcdir}/gf_implicit.c
    ${srcdir}/gf_prog.c ${srcdir}/gf_rtime.c ${srcdir}/gf_seq.c ${srcdir}/opttools.c ${srcdir}/optfoam.c ${srcdir}/of_util.c ${srcdir}/of_inlin.c
    ${srcdir}/of_cfold.c ${srcdir}/of_hfold.c ${srcdir}/of_emerg.c ${srcdir}/of_env.c ${srcdir}/of_cprop.c ${srcdir}/of_jflow.c
    ${srcdir}/of_peep.c ${srcdir}/of_deadv.c ${srcdir}/of_comex.c ${srcdir}/of_loops.c ${srcdir}/of_retyp.c ${srcdir}/of_deada.c
    ${srcdir}/of_rrfmt.c ${srcdir}/of_killp.c ${srcdir}/emit.c ${srcdir}/ccomp.c ${srcdir}/usedef.c ${srcdir}/flatten.c
    ${srcdir}/inlutil.c ${srcdir}/genc.c ${srcdir}/genlisp.c ${srcdir}/fortran.c ${srcdir}/gencpp.c)
#SOURCE_GROUP(lev3 FILES ${lev3})

# Level 2: Compiler Structures
SET(lev2 ${srcdir}/axlobs.c ${srcdir}/srcline.c ${srcdir}/token.c ${srcdir}/doc.c ${srcdir}/absyn.c ${srcdir}/absub.c
    ${srcdir}/ablogic.c ${srcdir}/fint.c ${srcdir}/output.c ${srcdir}/simpl.c ${srcdir}/compcfg.c ${srcdir}/depdag.c ${srcdir}/sefo.c ${srcdir}/syme.c
    ${srcdir}/freevar.c ${srcdir}/tform.c ${srcdir}/tfsat.c ${srcdir}/tposs.c ${srcdir}/tconst.c ${srcdir}/tqual.c ${srcdir}/stab.c ${srcdir}/lib.c
    ${srcdir}/archive.c ${srcdir}/foam.c ${srcdir}/flog.c ${srcdir}/dflow.c ${srcdir}/spesym.c ${srcdir}/abpretty.c ${srcdir}/version.c ${srcdir}/comsg.c
    ${srcdir}/loops.c)
#source_group(lev2 FILES ${lev2})

#
# Level 1: General Library
# NB: Should split foam files
SET(lev1 ${srcdir}/debug.c ${srcdir}/format.c ${srcdir}/fluid.c ${srcdir}/util.c ${srcdir}/store.c ${srcdir}/test.c ${srcdir}/foam_c.c
    ${srcdir}/foam_i.c ${srcdir}/foam_cfp.c ${srcdir}/foamopt.c ${srcdir}/compopt.c ${srcdir}/dword.c ${srcdir}/bigint.c ${srcdir}/bitv.c ${srcdir}/btree.c
    ${srcdir}/buffer.c ${srcdir}/dnf.c ${srcdir}/file.c ${srcdir}/fname.c ${srcdir}/list.c ${srcdir}/msg.c ${srcdir}/path.c ${srcdir}/srcpos.c ${srcdir}/strops.c
    ${srcdir}/symbol.c ${srcdir}/table.c ${srcdir}/ccode.c ${srcdir}/sexpr.c ${srcdir}/xfloat.c ${srcdir}/priq.c ${srcdir}/timer.c ${srcdir}/cfgfile.c
    ${srcdir}/termtype.c ${srcdir}/textansi.c ${srcdir}/texthp.c ${srcdir}/textcolour.c
    ${srcdir}/store1_t.c ${srcdir}/store2_t.c ${srcdir}/store3_t.c ${srcdir}/format_t.c ${srcdir}/fluid_t.c ${srcdir}/util_t.c
    ${srcdir}/bigint_t.c ${srcdir}/bitv_t.c ${srcdir}/btree_t.c ${srcdir}/buffer_t.c ${srcdir}/dnf_t.c ${srcdir}/file_t.c ${srcdir}/fname_t.c
    ${srcdir}/list_t.c ${srcdir}/msg_t.c ${srcdir}/strops_t.c ${srcdir}/symbol_t.c ${srcdir}/table_t.c ${srcdir}/ccode_t.c ${srcdir}/xfloat_t.c
    ${srcdir}/link_t.c ${srcdir}/priq_t.c ${srcdir}/float_t.c)
#SOURCE_GROUP(lev1 FILES ${lev1})

# Level 0: Portability
SET(lev0 ${srcdir}/stdc.c ${srcdir}/cport.c ${srcdir}/opsys.c
    ${srcdir}/cport_t.c ${srcdir}/opsys_t.c
    ${srcdir}/comsgdb.c ${srcdir}/axl_y.c)
#SOURCE_GROUP(lev0 FILES ${lev0})

FILE(GLOB headers ${srcdir}/*.h)
#SOURCE_GROUP(headers REGULAR_EXPRESSION .*[.]h)

ADD_EXECUTABLE(aldor ${lev0} ${lev1} ${lev2} ${lev3} ${lev4} ${headers})

ADD_EXECUTABLE(aldor_t ${lev0} ${lev1} ${lev2} ${lev3} ${lev4_t} ${headers})

add_dependencies(aldor unicl uniar)
add_dependencies(aldor_t unicl uniar)

IF(UNIX)
    IF(CYGWIN)
        set_target_properties(aldor PROPERTIES LINK_FLAGS -mno-cygwin)
    ELSE(CYGWIN)
        set_target_properties(aldor PROPERTIES LINK_FLAGS -lm)
    ENDIF(CYGWIN)
ENDIF(UNIX)

install(TARGETS aldor
        DESTINATION bin)
	
add_custom_command(TARGET aldor
	POST_BUILD 
	COMMAND mkdir ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/bin ${PROJECT_BINARY_DIR}/lib
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/aldor.conf ${PROJECT_BINARY_DIR}/include/aldor.conf
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/foam_c.h ${PROJECT_BINARY_DIR}/include/foam_c.h
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/foamopt.h ${PROJECT_BINARY_DIR}/include/foamopt.h
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/cconfig.h ${PROJECT_BINARY_DIR}/include/cconfig.h
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/optcfg.h ${PROJECT_BINARY_DIR}/include/optcfg.h
	COMMAND cp ${PROJECT_SOURCE_DIR}/${srcdir}/platform.h ${PROJECT_BINARY_DIR}/include/platform.h)
	
#libfoamlib library
#add

#libfoam library
add_library(libfoam ${srcdir}/stdc.c ${srcdir}/cport.c ${srcdir}/opsys.c ${srcdir}/btree.c ${srcdir}/store.c ${srcdir}/memclim.c ${srcdir}/bigint.c ${srcdir}/util.c ${srcdir}/foam_c.c ${srcdir}/foam_i.c ${srcdir}/output.c ${srcdir}/table.c ${srcdir}/xfloat.c ${srcdir}/foamopt.c ${srcdir}/compopt.c ${srcdir}/dword.c ${srcdir}/foam_cfp.c ${srcdir}/timer.c)

install(TARGETS libfoam
        DESTINATION lib)

install(FILES ${srcdir}/foam_c.h ${srcdir}/cconfig.h ${srcdir}/optcfg.h ${srcdir}/foamopt.h
	DESTINATION include)
	
#add_library(libfoam-runtime ${CMAKE_BINARY_DIR}/runtime.${objext})
#add_library(libfoam-runtime ${CMAKE_BINARY_DIR}/runtime.as)

add_custom_command(TARGET libfoam
	POST_BUILD
	COMMAND aldor -b${PROJECT_BINARY_DIR} -fc -fo -flsp -fao -Wruntime -Wcheck -Csmax=0 -Qno-cc -Q3 -Qinline-all -M no-ALDOR_W_OverRideLibraryFile ${PROJECT_SOURCE_DIR}/aldor/lib/libfoam/runtime.as
	COMMAND ${CMAKE_AR} r ${PROJECT_BINARY_DIR}/lib/libfoam.al runtime.ao
	COMMAND ${LIB} /out:libfoam.${libext} runtime.${objext}
	COMMAND mv libfoam.${libext} ${PROJECT_BINARY_DIR}/lib
	COMMAND rm runtime.ao runtime.lsp runtime.${objext} runtime.c 
	)
	
add_dependencies(libfoam libfoamlib)
	
set(AXL0Srcs ${foamlibsrcdir}/lang.as ${foamlibsrcdir}/machine.as ${foamlibsrcdir}/basic.as ${foamlibsrcdir}/foamcat.as ${foamlibsrcdir}/tuple.as ${foamlibsrcdir}/gener.as ${foamlibsrcdir}/boolean.as ${foamlibsrcdir}/segment.as ${foamlibsrcdir}/sinteger.as ${foamlibsrcdir}/sfloat.as ${foamlibsrcdir}/pointer.as ${foamlibsrcdir}/char.as ${foamlibsrcdir}/parray.as ${foamlibsrcdir}/array.as ${foamlibsrcdir}/list.as ${foamlibsrcdir}/langx.as ${foamlibsrcdir}/string.as)
set(AXL0Aos lang.ao machine.ao basic.ao foamcat.ao tuple.ao gener.ao boolean.ao segment.ao sinteger.ao sfloat.ao pointer.ao char.ao parray.ao array.ao list.ao langx.ao string.ao)
#message("AXL0Aos = ${AXL0Aos}")

set(AXL1Srcs ${foamlibsrcdir}/format.as ${foamlibsrcdir}/partial.as ${foamlibsrcdir}/oslow.as ${foamlibsrcdir}/fname.as ${foamlibsrcdir}/file.as ${foamlibsrcdir}/opsys.as ${foamlibsrcdir}/textwrit.as)
set(AXL1Aos format.ao partial.ao oslow.ao fname.ao file.ao opsys.ao textwrit.ao)

set(AOBJS ${AXL0Aos} ${AXL1Aos})
foreach(f ${AOBJS})
	string(REPLACE ".ao" ".${objext}" s ${f})
	list(APPEND AOS ${s})
endforeach(f)

add_custom_target(libfoamlib
	DEPENDS libfoamlib.al aldor)
	
add_custom_command(OUTPUT libfoamlib.al
	COMMAND aldor -fao -fo -M no-ALDOR_W_WillObsolete -M no-ALDOR_W_OverRideLibraryFile ${foamlibsrcdir}/lang.as
	COMMAND ${CMAKE_AR} r libfoamlib.al lang.ao
	COMMAND ${LIB} /out:libfoamlib.lib lang.${objext}
	COMMAND ${RM} lang.ao lang.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/machine.as
	COMMAND ${CMAKE_AR} r libfoamlib.al machine.ao
	COMMAND ${LIB} /out:libfoamlib.lib machine.${objext}
	COMMAND ${RM} machine.ao machine.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/basic.as
	COMMAND ${CMAKE_AR} r libfoamlib.al basic.ao
	COMMAND ${LIB} /out:libfoamlib.lib basic.${objext}
	COMMAND ${RM} basic.ao basic.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/foamcat.as
	COMMAND ${CMAKE_AR} r libfoamlib.al foamcat.ao
	COMMAND ${LIB} /out:libfoamlib.lib foamcat.${objext}
	COMMAND ${RM} foamcat.ao foamcat.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/tuple.as
	COMMAND ${CMAKE_AR} r libfoamlib.al tuple.ao
	COMMAND ${LIB} /out:libfoamlib.lib tuple.${objext}
	COMMAND ${RM} tuple.ao tuple.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/gener.as
	COMMAND ${CMAKE_AR} r libfoamlib.al gener.ao
	COMMAND ${LIB} /out:libfoamlib.lib gener.${objext}
	COMMAND ${RM} gener.ao gener.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/boolean.as
	COMMAND ${CMAKE_AR} r libfoamlib.al boolean.ao
	COMMAND ${LIB} /out:libfoamlib.lib boolean.${objext}
	COMMAND ${RM} boolean.ao boolean.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/segment.as
	COMMAND ${CMAKE_AR} r libfoamlib.al segment.ao
	COMMAND ${LIB} /out:libfoamlib.lib segment.${objext}
	COMMAND ${RM} segment.ao segment.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/sinteger.as
	COMMAND ${CMAKE_AR} r libfoamlib.al sinteger.ao
	COMMAND ${LIB} /out:libfoamlib.lib sinteger.${objext}
	COMMAND ${RM} sinteger.ao sinteger.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/sfloat.as
	COMMAND ${CMAKE_AR} r libfoamlib.al sfloat.ao
	COMMAND ${LIB} /out:libfoamlib.lib sfloat.${objext}
	COMMAND ${RM} sfloat.ao sfloat.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/pointer.as
	COMMAND ${CMAKE_AR} r libfoamlib.al pointer.ao
	COMMAND ${LIB} /out:libfoamlib.lib pointer.${objext}
	COMMAND ${RM} pointer.ao pointer.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/char.as
	COMMAND ${CMAKE_AR} r libfoamlib.al char.ao
	COMMAND ${LIB} /out:libfoamlib.lib char.${objext}
	COMMAND ${RM} char.ao char.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/parray.as
	COMMAND ${CMAKE_AR} r libfoamlib.al parray.ao
	COMMAND ${LIB} /out:libfoamlib.lib parray.${objext}
	COMMAND ${RM} parray.ao parray.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/array.as
	COMMAND ${CMAKE_AR} r libfoamlib.al array.ao
	COMMAND ${LIB} /out:libfoamlib.lib array.${objext}
	COMMAND ${RM} array.ao array.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/list.as
	COMMAND ${CMAKE_AR} r libfoamlib.al list.ao
	COMMAND ${LIB} /out:libfoamlib.lib list.${objext}
	COMMAND ${RM} list.ao list.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/langx.as
	COMMAND ${CMAKE_AR} r libfoamlib.al langx.ao
	COMMAND ${LIB} /out:libfoamlib.lib langx.${objext}
	COMMAND ${RM} langx.ao langx.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/string.as
	COMMAND ${CMAKE_AR} r libfoamlib.al string.ao
	COMMAND ${LIB} /out:libfoamlib.lib string.${objext}
	COMMAND ${RM} string.ao string.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/format.as
	COMMAND ${CMAKE_AR} r libfoamlib.al format.ao
	COMMAND ${LIB} /out:libfoamlib.lib format.${objext}
	COMMAND ${RM} format.ao format.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/partial.as
	COMMAND ${CMAKE_AR} r libfoamlib.al partial.ao
	COMMAND ${LIB} /out:libfoamlib.lib partial.${objext}
	COMMAND ${RM} partial.ao partial.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/oslow.as
	COMMAND ${CMAKE_AR} r libfoamlib.al oslow.ao
	COMMAND ${LIB} /out:libfoamlib.lib oslow.${objext}
	COMMAND ${RM} oslow.ao oslow.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/fname.as
	COMMAND ${CMAKE_AR} r libfoamlib.al fname.ao
	COMMAND ${LIB} /out:libfoamlib.lib fname.${objext}
	COMMAND ${RM} fname.ao fname.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/file.as
	COMMAND ${CMAKE_AR} r libfoamlib.al file.ao
	COMMAND ${LIB} /out:libfoamlib.lib file.${objext}
	COMMAND ${RM} file.ao file.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/opsys.as
	COMMAND ${CMAKE_AR} r libfoamlib.al opsys.ao
	COMMAND ${LIB} /out:libfoamlib.lib opsys.${objext}
	COMMAND ${RM} opsys.ao opsys.${objext}

	COMMAND aldor -b${PROJECT_BINARY_DIR} -fao -fo -lfoamlib -M no-aldor -b${PROJECT_BINARY_DIR}_W_OverRideLibraryFile ${foamlibsrcdir}/textwrit.as
	COMMAND ${CMAKE_AR} r libfoamlib.al textwrit.ao
	COMMAND ${LIB} /out:libfoamlib.lib textwrit.${objext}
	COMMAND ${RM} textwrit.ao textwrit.${objext}

	COMMAND ${CMAKE_RANLIB} libfoamlib.al
	COMMAND ${MV} libfoamlib.al ${PROJECT_BINARY_DIR}/lib
	COMMAND ${MV} libfoamlib.${libext} ${PROJECT_BINARY_DIR}/lib
	COMMAND ${CP} ${foamlibsrcdir}/foamlib.as ${PROJECT_BINARY_DIR}/include)

