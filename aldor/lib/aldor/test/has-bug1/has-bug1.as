#include "aldor"
define SomeType : Category == with {} ;

define SomeOtherType : Category == with {
    XXX : SomeType ;

    if XXX has OutputType then OutputType ;
}

