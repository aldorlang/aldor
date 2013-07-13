/**
 * Copyright © 2007 Pippijn van Steenhoven
 * License: GNU Affero General Public License version 3 or higher.
 */

#ifndef ERMYTH_SYSCALL_H
#define ERMYTH_SYSCALL_H

extern struct syscall
{
  enum action
  {
    deny,
    ignore,
    allow
  };

  char const *name;
  long const number;
  action access;

  static bool entering;
} calls[];

extern int const callcnt;

#endif
