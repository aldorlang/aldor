
-- Original author: Saul Youssef
--> testint

#include "axllib"
#pile

define FooCategory(Object:Type,Cat:Category):Category == with
    Foo: Object -> Cat with
             f: % -> %
	     default
	         f(x:%):% == error " "

