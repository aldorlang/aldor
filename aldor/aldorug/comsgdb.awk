# Create TeXable message file(s) from comsgdb.msg

BEGIN   {
    msgFile = "msghelp.tex"
    tag = ""
    text = ""
}

# Skip blank and comment lines

(NF == 0) || /^\$/ {
    next
}

# Start of a new message

/^AXL_/  {
    printMessage(tag, text)
    tag = $1
    text = substr($0,1+index($0, "\""))
    len = length(text)
    last = substr(text,len,1)
    if ("\\" == last || "\"" == last)
        text = substr(text,1,len-1)
    next
}

# Middle of message
    {
    len = length($0)
    last = substr($0,len,1)
    if ("\\" == last || "\"" == last)
        $0 = substr($0,1,len-1)
    text = text $0
    next
}

END {
    printMessage(tag, text)
}


function printMessage(tag, text)
{
    if (tag != "") {
        if (tag == "AXL_H_HelpCmd") {
            # handle someday
        }
	else if (tag == "AXL_H_HelpFileTypes") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpOptionSummary") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpHelpOpt" || tag == "AXL_H_HelpArgOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpDirOpt" || tag == "AXL_H_HelpFileOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpGoOpt" || tag == "AXL_H_HelpOptimOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpDebugOpt" || tag == "AXL_H_HelpCOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpLispOpt" || tag == "AXL_H_HelpMsgOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpDevOpt") {
	    # handle someday
	}
	else if (tag == "AXL_H_HelpMenuPointer") {
	    # handle someday
	}
        else
            printSingleMessage(tag, text, msgFile)
    }
}

function printSingleMessage(tag, text, file,    p)
{
    gsub(/_/,"\\_",tag)
    printf("\\errmsg{%s}{", tag) > file

    gsub(/\\t/," ",text)
    gsub(/\\"/,"\"",text)
    gsub(/_/,"\\_",text)
    gsub(/%/,"\\%",text)
    gsub(/#/,"\\#",text)
    gsub(/&/,"\\&",text)
    gsub(/\$/,"\\$",text)

    gsub(/>/,"$>$",text)
    gsub(/</,"$<$",text)

    gsub(/\=>/,"{\\tt =>}",text)
    gsub(/:=/, "{\\tt :=}",text)
    gsub(/\==/,"{\\tt ==}",text)
    gsub(/`='/,"`{\\tt =}'",text)
    gsub(/`:'/,"`{\\tt :}'",text)


    p = index(text,"\\n")
    while (p > 0) {
        displayLeadingBlanks(text, file)
        printf("%s", substr(text,1,p-1)) > file
        if (p == 1)
            printf("\\ ") > file
        printf("\\newline\n") > file
        text = substr(text,p+2)
        p = index(text,"\\n")
    }
    displayLeadingBlanks(text, file)
    printf("%s", text) > file

    printf("}\n") > file
}

function displayLeadingBlanks(text,file,     bl,len)
{
    len = length(text)
    bl = 1
    while (bl < len && substr(text,bl,1) == " ")
        bl++
    bl--
    if (bl > 0)
        printf("\\hspace*{%gin}",bl * .075) > file
}
