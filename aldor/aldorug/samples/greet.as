#include "aldor"

-- the following imports can be avoided by including "aldorio"
import from File;       -- so we can do input
import from TextReader; -- for stdin
import from TextWriter; -- for stdout
import from Character;  -- for newline
import from String;     -- for string literals

-- function to prompt for and return the user's name from the console
readName(): String == {
        stdout << "What is your name?" << newline;
        line := <<$String stdin;
        -- delete the trailing newline, and return the result
        line := [c for c in line | c ~= newline];
        line;
}

-- main function
greet(): () == {
        name := readName();
        stdout << "Hello " << name << ", and goodbye..." << newline;
}

greet();
