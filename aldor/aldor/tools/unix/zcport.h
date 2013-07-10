/*
 * zcport.h
 *
 * This file covers things which differ between C environments. 
 */

#ifndef _ZCPORT_H_
#define _ZCPORT_H_

#ifdef __STDC__
#  	define of(x) 	x
# else
#  	define of(x) 	()
#endif

#if defined(__STDC__) 
#        include <stdarg.h>
#else
#        if !defined(_VA_LIST) || defined(_HIDDEN_VA_LIST)
#            define _VA_LIST
             typedef char *  va_list;
#        endif
#        define va_start(list,arg)    list = (char *) (&(arg) + 1)
#        define va_end(list)
#        define va_arg(list, mode)    ((mode *)((list) += sizeof(mode)))[-1]
#endif

#endif /* _ZCPORT_H_ */
