-- Original from Saul Youssef

--> testint
--> testerrs

#include "axllib"

#if TestErrorsToo
Foo:Cross(C:Category,X:C) == (Ring,Integer)
#else
Foo:Cross(C:Category,X:(C@Category)) == (Ring,Integer)
#endif



