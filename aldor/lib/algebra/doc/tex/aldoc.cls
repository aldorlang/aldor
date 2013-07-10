%%
%% This is file `aldoc.cls',
%% generated with the docstrip utility.
%%
%% The original source files were:
%%
%% aldoc.dtx 
%% 
%%  Copyright (c)1994,1998,2000,2001 by ETH Zuerich
%% 
%%  Author: Niklaus Mannhart (mannhart@inf.ethz.ch)
%% 
%%  ---------------------------------------------------------------------
%%    The file `aldoc.dtx' file is free software; you can redistribute it
%%    and/or modify it under the terms of the GNU General Public License
%%    as published by the Free Software Foundation version 2 of the
%%    license.
%% 
%%    This class file is dsitributed in the hope that it will be usefull,
%%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
%%    General Public License for more details.
%%  ---------------------------------------------------------------------
%% 
%% IMPORTANT NOTICE:
%% 
%% You are not allowed to change this file.
%% 
%% You are allowed to distribute the original `aldoc.dtx' file under the
%% GNU condition.
%% 
%% Files included in this package:
%%   * README
%%   * aldoc.ins      installation file for aldoc
%%   * aldoc.dtx      the documented class file aldoc
%% 
%% If you receive only some of these files from someone, complain!
%% 
%% You are NOT ALLOWED to distribute this file alone.  You are NOT
%% ALLOWED to take money for the distribution or use of either this
%% file or a changed version, except for a nominal charge for copying
%% etc. (see GNU General Public License)
%% 
\NeedsTeXFormat{LaTeX2e} \ProvidesClass{aldoc}[2000/01/03 A\#
documentation class] \newif\ifhyperlink@set\hyperlink@setfalse
\DeclareOption{hyperref}{%
  \global\hyperlink@settrue
}
\DeclareOption*{%
        \PassOptionsToClass{\CurrentOption}{article}
}
\ProcessOptions
\LoadClass{article}
\setlength{\oddsidemargin}{-0.5cm}
\setlength{\evensidemargin}{-1.4cm}
\setlength{\marginparsep}{0cm}
\setlength{\marginparwidth}{0cm}
\setlength{\topmargin}{-1cm}
\setlength{\textheight}{\paperheight}
\addtolength{\textheight}{-4cm}
\addtolength{\textheight}{-\footskip}
\addtolength{\textheight}{-\hoffset}
\setlength{\textwidth}{\paperwidth}
\addtolength{\textwidth}{-4cm}
\usepackage{fancyhdr, epsfig, supertabular, makeidx, ttyverb}
\ifhyperlink@set% load hyperref package
    \RequirePackage{hyperref}
\fi
\setcounter{secnumdepth}{1}
\newenvironment{aldocpar}[1]{%
  \ifprint@it%
    \pagebreak[2]\par\bigskip\noindent
    \textbf{#1}\nopagebreak\par
      \begin{list}{}%
        {\@setlist}
        \item
  \fi}%
  {\ifprint@it\end{list}\smallskip\fi}%
\newcommand{\Aldocpar}[2]{%
  \ifprint@it%
    \begin{#1}
      #2
    \end{#1}
  \fi%
}%
\ifhyperlink@set% use hyperlink
    \newcommand{\alalias}[3]{\hyperlink{#1:#2}{\texttt{#3}}}
\else% else no hyperlink
    \newcommand{\alalias}[3]{\texttt{#3}}%
\fi
\newcommand{\alfunc}[2]{\alalias{#1}{#2}{#2}}%
\newcommand{\asfunc}[2]{\alfunc{#1}{#2}}%
\ifhyperlink@set% use hyperlink
    \newcommand{\altype}[1]{\hyperlink{#1}{\texttt{#1}}}%
\else% no hyperlink
    \newcommand{\altype}[1]{\texttt{#1}}%
\fi
\newcommand{\astype}[1]{\altype{#1}}
\newcommand{\altypes}[1]%
    {\addtocontents{toc}{\protect\contentsline{section}{#1}{}{}}}%
\newcommand{\albuiltin}[1]{\texttt{#1}}
\newcommand{\alexttype}[2]{\texttt{#1}}%
\ifhyperlink@set% use hyperlink
    \newcommand{\altarget}[1]{\hypertarget{\this:#1}{}}%
\else% no hyperlink
    \newcommand{\altarget}[1]{}%
\fi
\newcommand{\astarget}[1]{\altarget{#1}}
\newcommand{\shortthis}{}
\newcommand{\this}{}
\newcommand{\name}{}
\newif\iftypes@only\types@onlyfalse
\newcommand{\typesonly}{%
  \global\types@onlytrue%
}%
\newif\ifprint@it\print@ittrue
\newcommand{\all}{%
  \global\types@onlyfalse%
  \global\print@ittrue%
}%
\newcommand{\History}[3]{}
\newif\ifexport@header
\newcommand{\shortheader}{%
  \markboth{\shortthis}{}
}
\newcommand{\thistype}[2][]{%
  \def\arg{#1}
  \ifx\arg\empty
    \renewcommand{\shortthis}{#2}
  \else
    \renewcommand{\shortthis}{#1}
    \index{#1|see{#2}}
  \fi
  \newpage\clearpage\markboth{#2}{}\index{#2}
  \renewcommand{\this}{#2}
  \renewcommand{\name}{}
  \ifhyperlink@set%
    \hypertarget{\this}{}
    \addtocontents{toc}{\protect \contentsline {subsection}{#2}{\thepage}{\this}}
  \else
    \addtocontents{toc}{\protect \contentsline {subsection}{#2}{\thepage}{}}
  \fi
  \global\export@headertrue%
  \iftypes@only\global\print@ittrue\fi%
}
\newcommand{\alpage}[1]{%
  \iftypes@only\global\print@itfalse\fi%
  \ifprint@it%
    \newpage\clearpage\index{#1!\this}%
    \markright{#1}\renewcommand{\name}{#1}%
    \ifhyperlink@set%
      \hypertarget{\this:\name}{}%
    \fi
  \fi}
\newcommand{\aspage}[1]{\alpage{#1}}
\lhead[\fancyplain{}{\bfseries\rightmark}]%
      {\fancyplain{}{\bfseries\leftmark}}
\rhead[\fancyplain{}{\bfseries\leftmark}]%
      {\fancyplain{}{\bfseries\rightmark}}
\cfoot{\thepage}
\newcommand{\category}[1]{\multicolumn{3}{l}{#1}}
\newcommand{\@setlist}{%
  \setlength{\labelwidth}{0cm} \setlength{\leftmargin}{3ex}
  \setlength{\labelsep}{0cm}   \setlength{\rightmargin}{0cm}
  \setlength{\parsep}{0cm}
  \setlength{\topsep}{0cm} \setlength{\partopsep}{0cm}
}
\newenvironment{usage}{\begin{aldocpar}{Usage}}{\end{aldocpar}}
\newcommand{\Usage}[1]{\Aldocpar{usage}{#1}}%
\newenvironment{descr}{\begin{aldocpar}{Description}}{\end{aldocpar}}%
\newcommand{\Descr}[1]{\Aldocpar{descr}{#1}}%
\newenvironment{exports}[1][]{%
  \ifprint@it%
    \def\arg{#1}
    \pagebreak[2]
    \ifexport@header
      \par\bigskip\noindent
      \textbf{Exports}\nopagebreak\par
      \global\export@headerfalse
      \def\local{}
    \else \def\local{\par\medskip}
    \fi
    \ifx\arg\empty
      \begin{list}{}{\@setlist}
    \else
      \local\begin{list}{}{\@setlist}
        \item #1 \vspace{-1ex}\nopagebreak
    \fi
    \item \begin{supertabular}{lll}
  \fi}%
  {\ifprint@it\end{supertabular}\end{list}\smallskip\export@headerfalse\fi}
\newenvironment{alwhere}{%
  \ifprint@it%
    \medskip
    \begin{list}{}{\@setlist}
      \item where\nopagebreak
      \item \begin{supertabular}{lcl}%
  \fi}%
  {\ifprint@it\end{supertabular}\end{list}\smallskip\fi}
\newenvironment{aswhere}{\begin{alwhere}}{\end{alwhere}}
\newenvironment{retval}{\begin{aldocpar}{Returns}}{\end{aldocpar}}%
\newcommand{\Retval}[1]{\Aldocpar{retval}{#1}}%
\newenvironment{errors}{\begin{aldocpar}{Errors}}{\end{aldocpar}}%
\newcommand{\Errors}[1]{\Aldocpar{errors}{#1}}%
\newcommand{\alexp}[1]{\alfunc{\this}{#1}}
\newcommand{\asexp}[1]{\alexp{#1}}%
\newenvironment{alex}{\begin{aldocpar}{Example}}{\end{aldocpar}}%
\newcommand{\Alex}[1]{\Aldocpar{alex}{#1}}%
\newcommand{\Asex}[1]{\Aldocpar{asex}{#1}}%
\newenvironment{asex}[1]{\begin{alex}#1}{\end{alex}}%
\newenvironment{aloutput}{%
  \ifprint@it%
    \pagebreak[2]
    \begin{tabbing} \hspace{10ex}\= \kill%
  \fi}%
  {\ifprint@it\end{tabbing}\fi}
\newenvironment{asoutput}{%
  \ifprint@it%
    \pagebreak[2]
    \begin{tabbing} \hspace{10ex}\= \kill%
  \fi}%
  {\ifprint@it\end{tabbing}\fi}
\newenvironment{remarks}{\begin{aldocpar}{Remarks}}{\end{aldocpar}}%
\newcommand{\Remarks}[1]{\Aldocpar{remarks}{#1}}
\newcommand{\alsignature}[2]{%
  \ifprint@it%
    \pagebreak[2]\par\bigskip\noindent
    \textbf{Signature}\nopagebreak\par
    \begin{list}{}{\@setlist \advance \leftmargin by -0.2cm}
      \item
        \begin{tabular}{ll}
          {#1}: & {#2} \\
        \end{tabular}
    \end{list}%
  \fi%
}%
\newcommand{\Signature}[2]{\alsignature{\name}{{#1} $\to$ {#2}}}
\newcommand{\alconstant}[1]{\alsignature{\name}{#1}}
\newenvironment{signatures}{%
  \ifprint@it%
    \pagebreak[2]\par\bigskip\noindent
    \textbf{Signatures}\nopagebreak\par
    \begin{list}{}{\@setlist}
      \item
        \begin{tabular}{ll}%
  \fi}%
  {\ifprint@it\end{tabular}\end{list}\smallskip\fi}
\newcommand{\Signatures}[1]{%
  \ifprint@it
    \begin{signatures}
      #1
    \end{signatures}%
  \fi%
}
\newenvironment{params}{%
  \ifprint@it%
    \par\bigskip\noindent
    \pagebreak[2]
    \begin{tabular}{lll}
      \textbf{Parameter} & \textbf{Type} & \textbf{Description}\\ \hline
  \fi}%
  {\ifprint@it\end{tabular}\smallskip\fi}
\newcommand{\Params}[1]{%
  \ifprint@it%
    \begin{params}
      #1
    \end{params}%
  \fi%
}%
\newcommand{\@figurepath}{.}
\newcommand{\setfigpath}[1]{%
  \renewcommand{\@figurepath}{#1}}%
\newcommand{\asfigure}[3][]{%
  \ifprint@it%
    \begin{figure}[ht]
      \begin{center}
        \def\arg{#1}
        \ifx\arg\empty
          \epsfig{file=\@figurepath/#2.eps}
        \else
          \epsfig{file=\@figurepath/#2.eps, width=#1}
        \fi
        \def\arg{#3}
        \ifx\arg\empty
        \else \caption{#3}
        \fi
      \end{center}%
    \end{figure}%
    \par\noindent
  \fi%
}%
\newenvironment{@alseealso}{\begin{aldocpar}{See also}}{\end{aldocpar}}%
\newcommand{\alseealso}[1]{\Aldocpar{@alseealso}{#1}}%
%%%%%%%% That's it %%%%%%%%
\endinput
%%
%% End of file `aldoc.cls'.
