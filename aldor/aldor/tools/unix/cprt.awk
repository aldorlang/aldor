# cprt.awk
#
# Does some translations on a .c or .h file for latex
#
# The last argument should be the name of the tex file wanted.
# Example: awk -f cprt.awk *.c *.h aldorsrc
#
BEGIN   {
        filename = ARGV[--ARGC]
        atsign = 0
# make the needed style file
        system("rm -f cprt.sty")
        printSty("% Some external stuff")
        printSty("")
        printSty("% Modifications of headerfooter.sty")
        printSty("% Copyright (c) 1987 by Stephen Gildea")
        printSty("% Permission to copy all or part of this work is granted, provided")
        printSty("% that the copies are not made or distributed for resale, and that")
        printSty("% the copyright notice and this notice are retained.")
        printSty("%")
        printSty("")
        printSty("% Stephen Gildea, MIT Earth Resources Lab, Cambridge, Mass. 02139")
        printSty("% mit-erl!gildea, gildea@erl.mit.edu")
        printSty("")
        printSty("% \\pageheader{LEFT}{CENTER}{RIGHT} or \\pagefooter{LEFT}{CENTER}{RIGHT}")
        printSty("")
        printSty("% All of these commands take three arguments, which are printed at")
        printSty("% the left, center, and right of each page.  All three args must be")
        printSty("% provided even if some of them are empty.  The odd and even")
        printSty("% variations are only useful if you are using the twoside option.")
        printSty("")
        printSty("% Example: \\pagefooter{}{\\thepage}{}")
        printSty("")
        printSty("% Say \\pageheaderlinetrue if you want the header underlined.")
        printSty("")
        printSty("\\newif\\ifpageheaderline \\pageheaderlinefalse")
        printSty("")
        printSty("\\newdimen\\pageheaderlineskip  \\pageheaderlineskip=1.5pt")
        printSty("")
        printSty("\\def\\@headerline#1#2#3{%")
        printSty("        \\ifpageheaderline")
        printSty("          \\vbox{\\hbox to \\textwidth{\\rlap{#1}\\hss{#2}\\hss\\llap{#3}}%")
        printSty("          \\vskip\\pageheaderlineskip \\hrule}%")
        printSty("        \\else{\\rlap{#1}\\hss{#2}\\hss\\llap{#3}}\\fi}")
        printSty("")
        printSty("\\newcommand\\oddpageheader[3]{\\def\\@oddhead{\\@headerline{#1}{#2}{#3}}}")
        printSty("")
        printSty("\\newcommand\\evenpageheader[3]{\\def\\@evenhead{\\@headerline{#1}{#2}{#3}}}")
        printSty("")
        printSty("\\newcommand\\oddpagefooter[3]{\\def\\@oddfoot{\\rlap{#1}\\hss{#2}\\hss\\llap{#3}}}")
        printSty("")
        printSty("\\newcommand\\evenpagefooter[3]{\\def\\@evenfoot{\\rlap{#1}\\hss{#2}\\hss\\llap{#3}}}")
        printSty("")
        printSty("\\newcommand\\pageheader[3]{%")
        printSty("        \\oddpageheader{#1}{#2}{#3}\\evenpageheader{#1}{#2}{#3}}")
        printSty("")
        printSty("\\newcommand\\pagefooter[3]{%")
        printSty("        \\oddpagefooter{#1}{#2}{#3}\\evenpagefooter{#1}{#2}{#3}}")
        printSty("")
        printSty("% now some specific stuff that we've added")
        printSty("")
        printSty("\\newcount\\timehh\\newcount\\timemm")
        printSty("\\timehh=\\time")
        printSty("\\divide\\timehh by 60 \\timemm=\\time")
        printSty("\\count255=\\timehh\\multiply\\count255 by -60 \\advance\\timemm by \\count255")
        printSty("\\def\\rightnow{ \\ifnum\\timehh<10 0\\fi\\number\\timehh\\,:\\,\\ifnum\\timemm<10 0")
        printSty("\\fi\\number\\timemm}")
        printSty("\\def\\LineNum#1{\\hspace*{-.65in}\\makebox[.25in][r]{#1.}\\quad}")
        printSty("\\pagestyle{plain}")
        printSty("\\pagefooter{}{}{}")
        printSty("\\pageheaderlinetrue")
        printSty("\\setlength{\\textwidth}{6in}")
        printSty("\\setlength{\\textheight}{8.5in}")
        printSty("\\setlength{\\topmargin}{0in}")
        printSty("\\setlength{\\oddsidemargin}{.5in}")
        printSty("\\setlength{\\evensidemargin}{0in}")
        printSty("\\def\\baselinestretch{.8}")
        printSty("\\makeindex")
        close("cprt.sty")
# now make the document
        print "\\documentstyle[./cprt,twoside]{report}"
        print "\\begin{document}"
        print "\\tableofcontents\\newpage"
# lastCol1Line has contents of last line that had a nonblank in column 1
        lastCol1Line = ""
	srcFileActive = 0
	texFileActive = 0
}

# first line of new file
(FNR == 1)      {
        if (srcFileActive == 1) {
          print "} % end \\small"
          print "\\newpage"
	  srcFileActive = 0
        }
	if (texFileActive == 1) {
	  print "\\newpage"
	  texFileActive = 0
	}
	if (FILENAME ~ ".tex$") {
          print "\\oddpageheader{}{\\bf \\today\\  at \\rightnow}{\\bf \\thepage}"
          print "\\evenpageheader{\\bf \\thepage}{\\bf \\today\\  at \\rightnow}{}"
	  texFileActive = 1
	}
	else {
          print "\\oddpageheader{\\bf "FILENAME"}{\\bf \\today\\  at \\rightnow}{\\bf \\thepage}"
          print "\\evenpageheader{\\bf \\thepage}{\\bf \\today\\  at \\rightnow}{\\bf "FILENAME"}"
          print "\\addcontentsline{toc}{section}{"FILENAME"}"
          print "{\\small\\obeylines\\obeyspaces"
	  srcFileActive = 1
	}
}

# Pass tex source through as-is
(texFileActive == 1)	{
	print $0
	next
}

# empty line?
($1 == "")      {
        print "\\LineNum{"FNR"}"
        next
}

# #define?
("\#" == substr($1,1,1))        {
        if (1 == length($1)) {
                directive = $2
                macro = $3
        }
        else {
                directive = substr($1,2)
                macro = $2
        }
        if (directive == "define") {
                p = index(macro,"(")
                if (p)
                        macro = substr(macro,1,p-1)
                gsub(/_/,"\\_",macro)
                print "\\index{functions and defines ! "macro"}"
        }
}

# line with @ in it. Assume it has no ~.  (Note $+@ appear in Makefiles.)
/@/     {
        print "\\LineNum{"FNR"} \\verb~"$0"~"
        atsign = 1
}

# the default
        {
        if (! atsign)
          print "\\LineNum{"FNR"} \\verb@"$0"@"
        else
          atsign = 0

        if ("\{" == substr($0,1,1))
          indexFunctionName()

        if (substr($0,1,1) != " ")
          lastCol1Line = $0
}

# some special checks

/^enum / || /^struct / || /^union / {
        gsub(/_/,"\\_",$2)
        print "\\index{"$1"s !"$2"}"
}         

#/^typedef /     {
#        if ($2 == "struct")
#          td = $3
#        else
#          td = $2
#        gsub(/_/,"\\_",td)
#        print "\\index{"$1"s !"td"}"
#}

# and explicitly cycle
        {
        next
}


END     {
	if (srcFileActive == 1) {
          print "} % end \\small"
          print "\\newpage"
	}
	if (texFileActive == 1) {
	  print "\\newpage"
	}
        print "\\input{"filename".ind}"
        print "\\end{document}"
}

function indexFunctionName(line) {
        # assume first word of lastCol1Line is function name

        if (lastCol1Line == "")
          return

        p = index(lastCol1Line,"(")
        if (p)
          fun = substr(lastCol1Line,1,p-1)
        else
          fun = lastCol1Line
        gsub(/_/,"\\_",fun)
        print "\\index{functions and defines ! "fun"}"
        lastCol1Line = ""
}

function printSty(line) {
        print line >> "cprt.sty"
}
