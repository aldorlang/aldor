/****************************************************************************
 *
 * texthp.h: text utility functions for HP terminals
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 ****************************************************************************/

#ifndef _TEXTHP_H_
#define _TEXTHP_H_

#include "cport.h"

/* Expanded AF/AB escapes must not exceed HP_ESCAPE_MAXLEN in length */
#define HP_ESCAPE_MAXLEN 512

/*
 * HP terminal colour pairs. The RGB settings of the eight colour-pairs
 * may vary from system to system ...
 */
enum colourPair_enum {
   DefaultOnDefault = 0,
   RedOnDefault,
   GreenOnDefault,
   YellowOnDefault,
   BlueOnDefault,
   MagentaOnDefault,
   CyanOnDefault,
   DefaultOnYellow
};
typedef Enum(colourPair_enum) ColourHP;

/* Exported operations */

extern String txtBoldHP(void);
	/*
 	 * txtBoldHP() returns a string of escape codes for enabling the bold
 	 * terminal text attribute (extra bright). The only way to remove this
 	 * attribute is to write txtNormalHP() onto the stream. Note that we
 	 * assume term_type() returns a valid HP terminal (not UnknownTerm).
 	 */

extern String txtNormalHP(void);
	/*
 	 * txtNormalHP() returns a string of escape codes for disabling
 	 * all terminal text attributes currently in force. It also resets
 	 * the current colour pair to the default colour pair. Note that we
 	 * assume term_type() returns a valid HP terminal (not UnknownTerm).
 	 */

extern String txtColourHP(ColourHP);
	/*
 	 * txtColourHP(n) returns a string of escape codes for setting the
 	 * terminal foreground and background colours to colour-pair "n".
 	 * Colour-pair 0 corresponds to the default foreground/background.
 	 */
#endif /* _TEXTHP_H_ */
