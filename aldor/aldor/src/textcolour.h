/****************************************************************************
 *
 * textcolour.h: text highlighting
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#ifndef _TEXTCOLOUR_H_
#define _TEXTCOLOUR_H_

#include "axlobs.h"
#include "termtype.h"
#include "textansi.h"
#include "texthp.h"

/* Exported functions */
extern String tcolPrefix(CoMsgTag);
  /*
   * tcolPrefix(c) returns the text to be emitted before a message
   * of class c.
   */

extern String tcolPostfix(CoMsgTag);
  /*
   * tcolPostfix(c) returns the text to be emitted after a message
   * of class c.
   */

#endif /* _TEXT_COLOUR_H_ */
