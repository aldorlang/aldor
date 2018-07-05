#include "aldor"
#pile


Directory: with
    open: String -> %
    read: % -> Partial Pointer
    close: % -> ()
    readName: % -> Partial String
    listDirectory: String -> List FileName
== add
    Rep == Pointer
    import
        readdir: Pointer -> Pointer
	opendir: String -> Pointer
	closedir: Pointer -> MachineInteger
    from Foreign C "<dirent.h>"
    import
        direntName: Pointer -> Pointer
    from Foreign C "aldorlib.h"

    open(p: String): % == per opendir(p)
    read(dir: %): Partial Pointer ==
        import from Pointer
        ptr := readdir(rep dir)
	nil? ptr => failed
	[ptr]

    readName(dir: %): Partial String ==
        import from Partial Pointer, String
        ent := read dir
	failed? ent => failed
        [copy(direntName(retract ent) pretend String)]

    close(dir: %): () == closedir(dir pretend Pointer)

    listDirectory(path: String): List FileName ==
        import from Partial String, FileName
        dir := open path
	lst: List FileName := []
	done := false
        while not done repeat
	    name := readName dir
	    if failed? name then
	        done := true
	    else
	        lst := cons(filename(path, retract name), lst)
	close dir
	return lst
