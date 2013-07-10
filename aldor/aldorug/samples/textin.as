#include "aldor"

SI   ==> MachineInteger;
Char ==> Character;

-- Asserts that the charecter is a vowel
vowel?(c: Char): Boolean ==  {
    import from String;
    c = char "a" or c = char "e" or c = char "i"
    or c = char "o" or c = char "u";
}

-- Remove all the vowels from the input and
-- write the result on the output
removeVowels(tr: TextReader, tw: TextWriter): () == {
    import from Char, TextWriter, String;
    c: Char := read! tr;
    while (c ~= eof) repeat {
      if not vowel? c then write!(c, tw);
      c := read! tr;
    }
}

-- Prints a string and a newline on the standard output
printMessage(s:String):() == {
     import from String, TextWriter, Char;
     stdout << s << newline;
}

-- Constructs a string from the characters in the list
-- given its length and assuming that this list needs
-- to be reversed
convert(l: List Char, n: SI): String == {
      import from SI, Char;
      s: String := new(n, space);
      while (not empty? l) for i in 1..n repeat {
          c := first l; l := rest l;
          s.(n-i) := c;
      }
      s;
}

-- Generates the lines (as strings) from the input
lines(tr:TextReader): Generator String == generate {
     import from String;
     printMessage("entering lines");
     c: Char := read! tr; l: List Char := []; n: SI := 0;
     while (c ~= eof) repeat {
         l := []; n:=0;
         while (c ~= newline) repeat {
             l := cons(c,l); n := n + 1; c := read! tr;
         }
         yield convert(l,n); c := read! tr;
     }
     printMessage("leaving lines");
}

main(): () == {
    import from String;
    printMessage("entering test");
    f1: File := open("/etc/passwd",fileRead);
    f2: File := open("/tmp/passwd",fileWrite);
    tr: TextReader := f1::TextReader;
    tw: TextWriter := f2::TextWriter;
    removeVowels(tr,tw);
    close! f1;
    close! f2;
    f1: File := open("/tmp/passwd",fileRead);
    tr := f1::TextReader;
    for l in lines(tr) repeat {
        printMessage(l);
    }
    close! f1;
    printMessage("leaving test");
}

main();

