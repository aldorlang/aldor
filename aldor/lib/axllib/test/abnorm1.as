-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase abnorm
#pile

#include "axllib.as"

+++ List(S) provides an implementation of linked lists.
List: with
	macro S == Integer
	macro % == List

	+++ This is a pre-doc.
  	nil:       %

	cons:	   (S, %) -> %
		++ This is a post-doc.

	+++ This is a pre-doc followed by a...
	empty?:    % -> Boolean ++ post doc.

	+++ This is a multi-line pre-doc.
	+++ How about that.
	first:	   % -> S

	rest:	   % -> %	++ This is a funky
				++ multi-line
				++ post-doc.
    == add
	macro S == Integer
	macro % == List
