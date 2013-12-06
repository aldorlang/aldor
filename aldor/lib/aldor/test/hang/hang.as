#include "aldor"
#include "aldorio"

foo(): () == {
       import from Integer;
       import from List Integer;
       import from Assert List Integer;
       stdout << [1,2] << newline;
       stdout << [1;2] << newline;
       stdout << [2] << newline;
       assertEquals([2], [1;2]);
}

foo();
