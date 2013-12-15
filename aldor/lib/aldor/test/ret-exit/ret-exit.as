#include "aldor"
#include "aldorio"

myerror(s: String): Exit == never;

fn1(s: String): (String, String) == myerror(s);
fn2(s: String): (String, String) == return myerror(s);
fn3(s: String): (String, String) == if true then return myerror(s) else (s, s);
