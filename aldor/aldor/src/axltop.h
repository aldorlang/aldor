/*****************************************************************************
 *
 * axltop.h: All definitions for compiler.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This includes all definitions needed anywhwere in the compiler
 * and should only be used by the top level files.
 * 
 * Each of these includes the ones before it and the most specific one
 * possible should be used.
 *
 *   	stdc.h		-- Fixups to make enviroment closer to Stanard C
 *   	axlport.h	-- Additional portability definitions
 *
 *   	axlgen0.h	-- General functions on standard types
 *   	axlgen.h		-- Additional general types
 *
 *   	axlobs.h		-- Compiler structures
 *   	axlphase.h	-- Compiler phases
 *
 *   	axltop.h		-- Everything about the compiler (this file)
 */

#ifndef _AXLTOP_H_
#define _AXLTOP_H_

/* Pre-empt the include by axlphase.h to avoid too deep nesting. */
# include "axlgen.h"

# include "axlphase.h"

# include "axlcomp.h"

#endif	/* !_AXLTOP_H_ */
