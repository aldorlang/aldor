# Process \input statements in tex files, sending all to stdout

BEGIN   {
    self = "aldorughtex.awk"
    inputsRead = 0
    inputVerbatimsRead = 0
}

/\\input{/  {
    inputsRead++
    filename = substr($0,8)
    filename = substr(filename, 1, length(filename) - 1)
    if ( filename == "aldorugmacs" )
        next
    if ( filename == "aldorug.ind" )
        next
    if ( filename == "treedef" )
        next
    p = index(filename, ".")
    if (p == 0)
        filename = filename".tex"
    printf("%% Input from %s\n", filename)
    system( "awk -f " self " " filename )
    next
}

/\\ftext{/  {
    inputVerbatimsRead++
    filename = substr($0,8)
    filename = substr(filename, 1, length(filename) - 1)
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    next
}

/\\shin{/  {
    inputVerbatimsRead++
    filename = substr($0,7)
    filename = substr(filename, 1, length(filename) - 1)
    filename = filename".in"
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    next
}

/\\shio{/  {
    inputVerbatimsRead++
    filename = substr($0,7)
    filestub = substr(filename, 1, length(filename) - 1)
    filename = filestub".in"
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    filename = filestub".out"
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    next
}

/\\shrec{/  {
    inputVerbatimsRead++
    filename = substr($0,8)
    filename = substr(filename, 1, length(filename) - 1)
    filename = filename".rec"
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    next
}

/\\verbatiminput{/   {
    inputVerbatimsRead++
    filename = substr($0,16)
    filename = substr(filename, 1, length(filename) - 1)
    printf("%% Input from %s\n", filename)
    print "\\begin{verbatim}"
    while ( 1 == getline line < filename )
        print line
    close filename
    print "\\end{verbatim}"
    next
}

/\\newpage/ || /\\makeindex/ || /\\cleardoublepage/ || /\\pagenumbering/ || /\documentstyle/ || /\\begin{document}/ || /\\end{document}/ {
    next
}

/\\index/ || /\\tableofcontents/ || /\\listoffigures/ {
    next
}

    {
    print
    next
}

END {
    printf("%% Read in %d input files and %d verbatim input files\n", inputsRead, inputVerbatimsRead)
}
