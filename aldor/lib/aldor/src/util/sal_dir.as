#include "aldor"
#pile

BPtr ==> Ptr$Machine

Directory: with
    open: String -> %
    read: % -> Partial Pointer
    close: % -> ()
    readName: % -> Partial String
    listDirectory: String -> List FileName
== add
    Rep ==> BPtr
    import
        readdir: BPtr -> BPtr
	opendir: BPtr -> BPtr
	closedir: BPtr -> MachineInteger
    from Foreign C "<dirent.h>"
    import
        direntName: BPtr -> BPtr
    from Foreign C "aldorlib.h"

    open(p: String): % == per opendir(p pretend BPtr)
    read(dir: %): Partial Pointer ==
        import from Pointer
        ptr := readdir(rep dir) pretend Pointer
	nil? ptr => failed
	[ptr]

    readName(dir: %): Partial String ==
        import from Partial Pointer, String
        ent := read dir
	failed? ent => failed
        [copy(direntName( (retract ent) pretend BPtr) pretend String)]

    close(dir: %): () == closedir(dir pretend BPtr)

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
