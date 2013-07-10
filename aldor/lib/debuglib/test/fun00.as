
#include "axllib"
#include "debuglib"

start!()$NewDebugPackage;

main():() ==
{
   local lambda:DomainName;

   lambda := typeName( (SingleInteger, String) -> DoubleFloat );
   
query():() == {print << lambda << newline;}
query();
   print << lambda << newline;
}


main();
