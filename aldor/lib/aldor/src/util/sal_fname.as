#include "aldor"
#include "aldorio"
#pile

+++   This category provides an interface to names in the file system.
FileNameCategory : Category == with
        PrimitiveType
	OutputType

        coerce : String -> %
            ++ coerce(s) converts a string to a file name
            ++ according to operating system-dependent conventions.
        coerce : % -> String
            ++ coerce(fn) produces a string for a file name
            ++ according to operating system-dependent conventions.

        filename : (String, String, String) -> %
            ++ filename(d, n, e) creates a file name with
            ++ d as its directory, n as its name and e as its extension.
            ++ This is a portable way to create file names.
            ++ When d or e is the empty string, a default is used.

        filename : (String, String) -> %
            ++ filename(d, n) creates a file name with
            ++ d as its directory, n as its name and optional extension
            ++ This is a portable way to create file names.
            ++ When d is the empty string, a default is used.

        directory : % -> String
            ++ directory(f) returns the directory part of the file name.
        name : % -> String
            ++ name(f) returns the name part of the file name.
        extension : % -> String
            ++ extension(f) returns the type part of the file name.

        exists? : % -> Boolean
            ++ exists?(f) tests if the file exists in the file system.
        readable? : % -> Boolean
            ++ readable?(f) tests if the named file exist and can it be opened
            ++ for reading.
        writable? : % -> Boolean
            ++ writable?(f) tests if the named file be opened for writing.
            ++ The named file need not already exist.

        new : (String, String, String) -> %
            ++ new(d, pref, e) constructs the name of a new writable file with
            ++ d as its directory, pref as a prefix of its name and
            ++ e as its extension.
            ++ When d or t is the empty string, a default is used.
            ++ An error occurs if a new file cannot be written in the given
            ++ directory.

+++   This domain provides an interface to names in the file system.
+++
FileName : FileNameCategory == add
        Rep == Record(p: String, name: String, ext: String)
	import from Rep
        import from MachineInteger

	default f, f1, f2: %
        f1 = f2: Boolean           == never

        coerce(f : %) : String     ==
		 filePart := name(f) + (if extension f = "" then "" else ("." + extension(f)))
		 if directory(f) = "" then filePart else directory(f) + "/" + filePart

        coerce(s : String) : %     ==
	    (flg1, lastSlash, c) := linearReverseSearch(char "/", s)
	    (flg2, lastDot, c) := linearReverseSearch(char ".", s)
	    if flg1 and flg2 and lastDot > lastSlash then
	        filename(substring(s, 0, lastSlash),
		         substring(s, lastSlash+1, lastDot - lastSlash - 1),
			 substring(s, lastDot + 1))
            else if flg2 then
	        filename("", substring(s, 0, lastDot), substring(s, lastDot + 1))
	    else if flg1 then
	        filename(substring(s, 0, lastSlash), substring(s, lastSlash+1), "")
	    else
		filename("", s, "")

        filename(d: String, n: String, e: String): % == per [d, n, e]
        filename(d: String, full: String): % ==
	    (flg2, lastDot, c) := linearReverseSearch(char ".", full)
	    not flg2 => filename(d, full, "");
	    filename(d, substring(full, 0, lastDot), substring(full, lastDot+1))

        directory(f : %) : String   == rep(f).p
        name(f : %) : String        == rep(f).name
        extension(f : %) : String   == rep(f).ext
        exists? f: Boolean    == never
        readable? f: Boolean  == never
        writable? f: Boolean  == never

        new(d: String, pref: String, e: String): % == filename(d, pref, e)

	(o: TextWriter) << (f: %): TextWriter == o << "{fn: " << directory f << "/" << name f << "." << extension f << "}"

#if ALDORTEST
---------------------- test sal_command.as --------------------------
#include "aldor"
#include "aldortest"
#pile

local testFile(): () ==
    import from Assert String
    fname: FileName := "/foo/bar/wibble.txt"::FileName
    assertEquals("/foo/bar", directory fname);
    assertEquals("wibble", name fname);
    assertEquals("txt", extension fname);

    fname: FileName := "wibble.txt"::FileName
    assertEquals("", directory fname);
    assertEquals("wibble", name fname);
    assertEquals("txt", extension fname);

    fname: FileName := "wibble"::FileName
    assertEquals("", directory fname);
    assertEquals("wibble", name fname);
    assertEquals("", extension fname);
    assertEquals("wibble", fname::String)

testFile()

#endif
