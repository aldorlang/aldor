/*****************************************************************************
 *
 * gf_reference.c: Foam code generation for reference expressions
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_REFERENCE_H_
#define _GF_REFERENCE_H_

 
extern Foam genReference(AbSyn);
extern Foam genReferenceFrFoam(Foam, TForm, AbSyn);
extern AInt gen0MakeRefFormat();

#endif /*!_GF_REFERENCE_H_*/
