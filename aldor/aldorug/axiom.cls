


\ProvidesClass  {axiom}
  [1999/10/26 v2.0 axiom class]


\def\@ptsize{0}
\DeclareOption{10pt}{\def\@ptsize{0}}
\DeclareOption{11pt}{\def\@ptsize{1}}
\DeclareOption{12pt}{\def\@ptsize{2}}
\DeclareOption{twoside}{\@twosidetrue \@mparswitchtrue}
\DeclareOption{draft}{\overfullrule 5pt}
\DeclareOption{ps}{\input{axiomps.sty}}    % get the PostScript stuff, if requested
%\def\ds@epsf{\input{epsf.sty}}     % get the dvips      stuff, if requested

\ProcessOptions

\DeclareOldFontCommand{\rm}{\normalfont\rmfamily}{\mathrm}
\DeclareOldFontCommand{\sf}{\normalfont\sffamily}{\mathsf}
\DeclareOldFontCommand{\tt}{\normalfont\ttfamily}{\mathtt}
\DeclareOldFontCommand{\bf}{\normalfont\bfseries}{\mathbf}
\DeclareOldFontCommand{\it}{\normalfont\itshape}{\mathit}
\DeclareOldFontCommand{\sl}{\normalfont\slshape}{\@nomath\sl}
\DeclareOldFontCommand{\sc}{\normalfont\scshape}{\@nomath\sc}
\DeclareRobustCommand*\cal{\@fontswitch\relax\mathcal}
\DeclareRobustCommand*\mit{\@fontswitch\relax\mathnormal}


\lineskip 1pt \normallineskip 1pt
\def\baselinestretch{1}

\@ifundefined{ixpt}{\def\ixpt{}}{}

% ------------------------------------------------------------------------
% Font Setup
% ------------------------------------------------------------------------

\def\normalsize{\@setsize\normalsize{11pt}\xipt\@xipt
\abovedisplayskip 11pt plus3pt minus6pt\belowdisplayskip \abovedisplayskip
\abovedisplayshortskip \z@ plus3pt\belowdisplayshortskip 6.5pt plus3.5pt
minus3pt\let\@listi\@listI}

\def\small{\@setsize\small{8pt}\ixpt\@ixpt
\abovedisplayskip 10pt plus2pt minus5pt\belowdisplayskip \abovedisplayskip
\abovedisplayshortskip \z@ plus3pt\belowdisplayshortskip 6pt plus3pt minus3pt
\def\@listi{\leftmargin\leftmargini \topsep 6pt plus 2pt minus 2pt\parsep 3pt
plus 2pt minus 1pt \itemsep \parsep}}

\def\footnotesize{\small \@ifundefined{ugUsingPS}{}{\def\TYsize{\SMTYfont}}}
\def\scriptsize{\@setsize\scriptsize{9.5pt}\viiipt\@viiipt}
\def\tiny{\@setsize\tiny{6pt}\vipt\@vipt}
\def\large{\@setsize\large{14pt}\xiipt\@xiipt}
\def\Large{\@setsize\Large{18pt}\xivpt\@xivpt}
\def\LARGE{\@setsize\LARGE{22pt}\xviipt\@xviipt}
\def\huge{\@setsize\huge{25pt}\xxpt\@xxpt}
\def\Huge{\@setsize\Huge{30pt}\xxvpt\@xxvpt}
\def\HugeSf{\Huge\bf}

% ------------------------------------------------------------------------
% These macros allow CM or PS fonts to be used.
% ------------------------------------------------------------------------

% Display Math - always Computer Modern
\def\DMsize{\@normalsize\baselineskip 10pt}


% Chapter Number
\@ifundefined{ugUsingPS}%
  {\def\CNsize{\LARGE\sf\baselineskip 18pt}}%    CM version
  {\def\CNsize{\baselineskip 18pt\CNfont}}%      PS version

% Chapter Title
\@ifundefined{ugUsingPS}%
  {\def\CTsize{\Huge\bf\baselineskip 42pt}}%     CM version
  {\def\CTsize{\baselineskip 40pt\CTfont}}%      PS version

% Section Number and Title
\@ifundefined{ugUsingPS}%
  {\def\SNTsize{\Large\sf\baselineskip 15pt}}%   CM version
  {\def\SNTsize{\baselineskip 15pt\SNTfont}}%    PS version

% SubSection Number and Title
\@ifundefined{ugUsingPS}%
  {\def\SSNTsize{\large\sf\baselineskip 13pt}}%  CM version
  {\def\SSNTsize{\baselineskip 13pt\SSNTfont}}%  PS version

% Axiom Input
\@ifundefined{ugUsingPS}%
  {\def\Isize{\small\xpt\@xpt\tt}}%              CM version
  {\def\Isize{\baselineskip 10pt\Ifont}}%        PS version

% Small Axiom Input
\@ifundefined{ugUsingPS}%
  {\def\SIsize{\ixpt\tt\baselineskip 10pt}}%     CM version
  {\def\SIsize{\baselineskip 10pt\SIfont}}%      PS version

% VERbatim text
\@ifundefined{ugUsingPS}%
  {\def\VERsize{\xipt\tt\baselineskip 13pt}}%    CM version
  {\def\VERsize{\baselineskip 13pt\VERfont}}%    PS version

% Example COMments
\@ifundefined{ugUsingPS}%
  {\def\COMsize{\small}\def\SMTYfont{\sf}}%      CM version
  {\def\COMsize{\def\TYsize{\SMTYfont}\baselineskip 10pt\COMfont\ixpt{}}}%    PS version

% Figure Caption
\@ifundefined{ugUsingPS}%
  {\def\FCsize{\xpt\sf}}%                        CM version
  {\def\FCsize{\baselineskip 10pt\FCfont}}%      PS version

% Running Foot
\@ifundefined{ugUsingPS}%
  {\def\RFsize{\ixpt\bf}}%                       CM version
  {\def\RFsize{\baselineskip 9pt\RFfont}}%       PS version

% Type Element (\axiomType and \spadtype and friends)
\@ifundefined{ugUsingPS}%
  {\def\TYsize{\sf}}%                            CM version
  {\def\TYsize{\baselineskip 10pt\TYfont}}%      PS version

% Type Element for "Type:" (in Axiom output)
\@ifundefined{ugUsingPS}%
  {\def\TYTsize{\small\rm}}%                       CM version
  {\def\TYTsize{\baselineskip 10pt\TYTfont}}%      PS version

\normalsize

% ------------------------------------------------------------------------
% Page Parameters
% ------------------------------------------------------------------------

%\newdimen\commentWidth \commentWidth=11pc
\newdimen\commentWidth \commentWidth=30mm
%\newdimen\colGutterWidth \colGutterWidth=1pc
\newdimen\colGutterWidth \colGutterWidth=4mm
%MCD: This looks like textwidth - commentWidth - colGutterWidth
\newdimen\xmpWidth \xmpWidth= 121mm
%\newdimen\xmpWidth \xmpWidth=29pc
\newdimen\xmpLessStepWidth \xmpLessStepWidth=112mm
%\newdimen\xmpLessStepWidth \xmpLessStepWidth=27pc

\newdimen\baseLeftSkip
\newdimen\savedBaseLeftSkip
\baseLeftSkip=\commentWidth \advance\baseLeftSkip by \colGutterWidth
\leftskip=\baseLeftSkip
\savedBaseLeftSkip=\baseLeftSkip

% 41pc is approx 170mm
%\setlength{\textwidth}{41pc}
%\setlength{\textheight}{7.82in}
\setlength{\textwidth}{155mm}
\setlength{\textheight}{185mm}
\setlength{\topmargin}{\z@}
\setlength{\oddsidemargin}{\z@}
\setlength{\evensidemargin}{\z@}
\parskip .5\baselineskip \parindent \z@
\def\baselinestretch{1.0}
\raggedbottom

\def\useFullPage{\baseLeftSkip=\z@ \xmpWidth=\hsize \leftskip=\z@%
\@totalleftmargin=\z@}

\fboxrule = .5pt

\if@twoside \marginparwidth 100pt \else \marginparwidth 83pt \fi
\marginparsep 10pt \headheight 12pt \headsep 25pt \footskip 30pt
\columnsep 1pc \columnseprule 0pt

\footnotesep 7.7pt
\skip\footins 10pt plus 4pt minus 2pt
\floatsep 12pt plus 2pt minus 2pt \textfloatsep 20pt plus 2pt minus 4pt
\intextsep 12pt plus 2pt minus 2pt  \dblfloatsep 12pt plus 2pt
minus 2pt \dbltextfloatsep 20pt plus 2pt minus 4pt 
\@fptop 0pt plus 1fil \@fpsep 8pt plus 2fil \@fpbot 0pt plus 1fil
\@dblfptop 0pt plus 1fil \@dblfpsep 8pt plus 2fil \@dblfpbot 0pt plus 1fil
\marginparpush 5pt
\partopsep 3pt plus 1pt minus 2pt
\@lowpenalty 51 \@medpenalty 151 \@highpenalty 301
\@beginparpenalty -\@lowpenalty \@endparpenalty -\@lowpenalty \@itempenalty
-\@lowpenalty

% the following are narrow and wide definition term and definition
% macros.

\newdimen\displayTermWidth
%\displayTermWidth=5pc
\displayTermWidth=21mm
\newdimen\displayGapWidth
\displayGapWidth=5mm
\newdimen\displayDefWidth
%\displayDefWidth=22pc
%\displayDefWidth=\xmpWidth -\displayTermWidth -\displayGapWidth
\displayDefWidth=95mm

\newcommand{\dtdd}[2]{\par%
\begin{tabular}{@{}p{\displayTermWidth}@{\hspace{\displayGapWidth}}p{\displayDefWidth}@{}}%
{\tt #1}&#2%
\end{tabular}\par}

\newdimen\wideDisplayTermWidth
%\wideDisplayTermWidth=8pc
\wideDisplayTermWidth=35mm
\newdimen\wideDisplayDefWidth
%\wideDisplayDefWidth=18pc
\wideDisplayDefWidth=81mm %\xmpWidth -\wideDisplayTermWidth -\displayGapWidth
\newdimen\wideDisplayDefAuxWidth
%\wideDisplayDefAuxWidth=10pc
\wideDisplayDefAuxWidth=32mm
\newdimen\displayOptionWidth
\displayOptionWidth=6mm

\newcommand{\widedtdd}[2]{\par%
\begin{tabular}{@{}p{\wideDisplayTermWidth}@{\hspace{\displayGapWidth}}p{\wideDisplayDefWidth}@{}}%
{\tt #1}&#2%
\end{tabular}\par}

\newcommand{\tabledtdd}[6]{\par%
\begin{tabular}{@{}p{\wideDisplayTermWidth}@{\hspace{\displayGapWidth}}p{\wideDisplayDefAuxWidth}*{4}{p{\displayOptionWidth}@{\hspace{\displayGapWidth}}}@{}}%
{\tt #1}&{\raggedright#2\par\vspace{-\baselineskip}}&#3&#4&#5&#6%
\end{tabular}\par}

% change in following is the \cleardoublepage
\def\twocolumn{\cleardoublepage \global\columnwidth\textwidth
   \global\advance\columnwidth -\columnsep \global\divide\columnwidth\tw@
   \global\hsize\columnwidth \global\linewidth\columnwidth
   \global\@twocolumntrue \global\@firstcolumntrue
   \@dblfloatplacement\@ifnextchar[{\@topnewpage}{}}

%% part and chapter headings are mirror images.

% This defines the part headings.
% \def\partHead#1#2{{%
%  \hbadness=10001\leftskip=\z@%
%  \noindent \vrule height 1.7mm width \xmpWidth\hfill\break%
%  \vskip -\baselineskip\vskip 5pt%
%  \hbox to \xmpWidth{\hfill #1} \hfill\hbox{}%
%  \par \vskip -\parskip \vskip -\baselineskip%
%  \vskip 28mm \leftskip=0pt%
%  \hrule width \textwidth height .5pt depth0pt%
%  \vskip 6pt \par\noindent%
%  \hfill\begin{minipage}[t]{23pc}\raggedleft\CTsize #2\end{minipage}
%  \vskip 12mm
%  }}

% This defines the chapter headings.
\def\chapterHead#1#2{{%
 \hbadness=10001\leftskip=\baseLeftSkip%
 \noindent \vrule height 1.7mm width \xmpWidth\hfill\break%
 \par \vskip -\parskip \vskip -\baselineskip\vskip 5pt%
 {#1}\hfill\hbox{}%
 \par \vskip -\parskip \vskip -\baselineskip%
 \vskip 35mm \leftskip=0pt%
 \hrule width \textwidth height .5pt depth0pt%
 \vskip 6pt \par\noindent%
 \begin{minipage}[t]{130mm}\raggedright\CTsize #2\end{minipage}
 \vskip 12mm
 }}

\def\ssectionmark#1{\markright{#1~~\pageBullet}}

% modify chapter look
\def\@makechapterhead#1{\chapterHead{\protect\CNsize\@chapapp \ \thechapter}{#1}}
\def\@makeschapterhead#1{\chapterHead{\hbox{}}{#1}}

% use this for unnumbered chapters
\def\schapter#1{\cleardoublepage \global\@topnum\z@
\@afterindentfalse\if@twocolumn \@topnewpage[\@makeschapterhead{#1}]
 \else \@makeschapterhead{#1}
 \addcontentsline{toc}{chapter}{#1}
 \chaptermark{#1}\ssectionmark{#1}\@afterheading\fi}

\def\chapter{\cleardoublepage \global\@topnum\z@
\@afterindentfalse \secdef\@chapter\@schapter}

\def\@chapter[#1]#2{\ifnum \c@secnumdepth >\m@ne
 \refstepcounter{chapter}
 \typeout{\@chapapp\space\thechapter.}
 \addcontentsline{toc}{chapter}{\protect
 \numberline{\thechapter}#1}\else
 \addcontentsline{toc}{chapter}{#1}\fi
 \chaptermark{#1}
 \addtocontents{lof}{\protect\addvspace{10pt}}
\addtocontents{lot}{\protect\addvspace{10pt}} \if@twocolumn
\@topnewpage[\@makechapterhead{#2}]
 \else \@makechapterhead{#2}
 \@afterheading \fi}

\def\@schapter#1{\if@twocolumn \@topnewpage[\@makeschapterhead{#1}]
 \else \@makeschapterhead{#1}
 \@afterheading\fi}

\newbox\sectionBox

\def\subsubsection#1{\par%
\allowbreak\noindent\hspace*{-\baseLeftSkip}%
\setbox\sectionBox=\hbox{%
\begin{minipage}[t]{\commentWidth}%
\raggedright\normalsize\sf{#1}\newline%
\end{minipage}\hbox to \colGutterWidth{\hfill}}\copy\sectionBox%
\par%
\vskip -\baselineskip\vskip -\parskip%
\vskip -\dp\sectionBox\vskip -\ht\sectionBox%
\nobreak\noindent}%

\def\pseudoSection#1{\par\vskip -\parskip\vskip 10mm
\allowbreak\noindent\hspace*{-\baseLeftSkip}%
\setbox\sectionBox=\hbox{%
\begin{minipage}[t]{\commentWidth}%
\raggedright\SSNTsize{#1}\newline%
\raise 11pt \hbox{\vrule height .5pt width \commentWidth} \hfill%
\end{minipage}\hbox to \colGutterWidth{\hfill}}\copy\sectionBox%
\markright{{#1~~\pageBullet}}%
\par%
\vskip -\baselineskip\vskip -\parskip%
\vskip -\dp\sectionBox\vskip -\ht\sectionBox%
\nobreak\noindent}%

\def\section#1{\par\vskip -\parskip\vskip 10mm \refstepcounter{section}%
\allowbreak\noindent\hspace*{-\baseLeftSkip}%
\setbox\sectionBox=\hbox{%
\begin{minipage}[t]{\commentWidth}%
\raggedright\SNTsize\thesection\newline{#1}\newline%
\raise 11pt \hbox{\vrule height .5pt width \commentWidth} \hfill%
\end{minipage}\hbox to \colGutterWidth{\hfill}}\copy\sectionBox%
\addcontentsline{toc}{section}{\protect\numberline{\thesection}{#1}}%
\sectionmark{#1}%
\par%
\vskip -\baselineskip\vskip -\parskip%
\vskip -\dp\sectionBox\vskip -\ht\sectionBox%
\nobreak\noindent}%

\def\widesection#1{\par\vskip -\parskip\vskip 10mm \refstepcounter{section}%
\allowbreak\noindent\hspace*{-\baseLeftSkip}%
{\SNTsize\thesection\nobreak\newline\nobreak}
\setbox\sectionBox=\hbox{\raggedright\SNTsize{#1}}
\hspace*{-\baseLeftSkip}\copy\sectionBox\nobreak\newline\nobreak%
\hspace*{-\baseLeftSkip}%
\raise 11pt \hbox{\vrule height .5pt width \wd\sectionBox}%
\addcontentsline{toc}{section}{\protect\numberline{\thesection}{#1}}%
\sectionmark{#1}%
\par%
\vskip -\baselineskip\vskip -2\parskip%
\nobreak\noindent}%

\def\subsection#1{\par\vskip -\parskip\vskip 10mm \refstepcounter{subsection}%
\noindent\hspace*{-\baseLeftSkip}%
\setbox\sectionBox=\hbox{%
\begin{minipage}[t]{\commentWidth}%
\raggedright\SSNTsize\thesubsection\newline{#1}\newline%
\raise 11pt \hbox{\vrule height .5pt width \commentWidth} \hfill%
\end{minipage}\hbox to \colGutterWidth{\hfill}}\copy\sectionBox%
\par%
\vskip -\baselineskip\vskip -\parskip%
\vskip -\dp\sectionBox\vskip -\ht\sectionBox%
\nobreak\noindent}%

\def\widesubsection#1{\par\vskip -\parskip\vskip 10mm \refstepcounter{subsection}%
\allowbreak\noindent\hspace*{-\baseLeftSkip}%
{\SSNTsize\thesubsection\nobreak\newline\nobreak}
\setbox\sectionBox=\hbox{\raggedright\SSNTsize{#1}}
\hspace*{-\baseLeftSkip}\copy\sectionBox\nobreak\newline\nobreak%
\hspace*{-\baseLeftSkip}%
\raise 11pt \hbox{\vrule height .5pt width \wd\sectionBox}%
\par%
\vskip -\baselineskip\vskip -2\parskip%
\nobreak\noindent}%

\def\paragraph{\@startsection
 {paragraph}{4}{\z@}{3.25ex plus 1ex minus .2ex}{-1em}{\normalsize\bf}}
\def\subparagraph{\@startsection
 {subparagraph}{4}{\parindent}{3.25ex plus 1ex minus
 .2ex}{-1em}{\normalsize\bf}}

\def\chaptermark#1{}

\setcounter{secnumdepth}{2}

\def\appendix{\par
 \setcounter{chapter}{0}
 \setcounter{section}{0}
 \def\@chapapp{APPENDIX}
 \def\thechapter{\Alph{chapter}}
 \cleardoublepage \thispagestyle{empty} \if@twocolumn \onecolumn
   \@tempswatrue \else \@tempswafalse \fi%
 \addcontentsline{toc}{part}{Appendices}\markboth{}{}
 \typeout{APPENDICES.}
 \chapterHead{\protect\CNsize APPENDICES}{\hbox{}}
 \@endpart}

\@totalleftmargin=\baseLeftSkip
\leftmargini   2.5em
\leftmarginii  2.2em
\leftmarginiii 1.87em
\leftmarginiv  1.7em
\leftmarginv   1em
\leftmarginvi  1em
%
\leftmargin\leftmargini
\labelsep .5em
\labelwidth\leftmargini\advance\labelwidth-\labelsep

% change \@listI a bit
\def\@listI{\leftmargin\leftmargini \parsep 4.5pt plus 2pt minus 1pt\topsep
9pt plus 3pt minus 5pt\itemsep 0pt plus 2pt minus 1pt}

\let\@listi\@listI
\@listi
\let\@listi\@listI
\@listi
\def\@listii{\leftmargin\leftmarginii \advance\rightmargin by \leftmarginii
 \labelwidth\leftmarginii\advance\labelwidth-\labelsep
 \topsep 4.5pt plus 2pt minus 1pt
 \parsep 2pt plus 1pt minus 1pt
 \itemsep \parsep}
\def\@listiii{\leftmargin\leftmarginiii
 \labelwidth\leftmarginiii\advance\labelwidth-\labelsep
 \topsep 2pt plus 1pt minus 1pt
 \parsep \z@ \partopsep 1pt plus 0pt minus 1pt
 \itemsep \topsep}
\def\@listiv{\leftmargin\leftmarginiv
 \labelwidth\leftmarginiv\advance\labelwidth-\labelsep}
\def\@listv{\leftmargin\leftmarginv
 \labelwidth\leftmarginv\advance\labelwidth-\labelsep}
\def\@listvi{\leftmargin\leftmarginvi
 \labelwidth\leftmarginvi\advance\labelwidth-\labelsep}

% ------------ end old ug11.sty stuff here --------------------------------------


\def\labelenumi{\theenumi.}
\def\theenumi{\arabic{enumi}}
\def\labelenumii{(\theenumii)}
\def\theenumii{\alph{enumii}}
\def\p@enumii{\theenumi}
\def\labelenumiii{\theenumiii.}
\def\theenumiii{\roman{enumiii}}
\def\p@enumiii{\theenumi(\theenumii)}
\def\labelenumiv{\theenumiv.}
\def\theenumiv{\Alph{enumiv}}
\def\p@enumiv{\p@enumiii\theenumiii}

\def\labelitemi{$\bullet$}
\def\labelitemii{\bf --}
\def\labelitemiii{$\ast$}
\def\labelitemiv{$\cdot$}

\def\verse{\let\\=\@centercr
 \list{}{\itemsep\z@ \itemindent -1.5em\listparindent \itemindent
 \rightmargin\leftmargin\advance\leftmargin 1.5em}\item[]}
\let\endverse\endlist
\def\quotation{\vskip -\parskip\list{}{\listparindent \z@
 \itemsep 2pt \topsep \parskip \parsep \z@ \partopsep \z@
 \itemindent\listparindent \linewidth\xmpWidth
 \rightmargin\leftmargin}\item[]}
\def\endquotation{\endlist\vskip -\parskip}
\def\quote{\list{}{\rightmargin\leftmargin}\item[]}
\let\endquote=\endlist


\def\titlepage{\@restonecolfalse\if@twocolumn\@restonecoltrue\onecolumn
 \else \newpage \fi \thispagestyle{empty}\c@page\z@}
\def\endtitlepage{\if@restonecol\twocolumn \else \newpage \fi}

\arraycolsep 5pt \tabcolsep 6pt \arrayrulewidth .4pt \doublerulesep 2pt
\tabbingsep \labelsep
\skip\@mpfootins = \skip\footins
\fboxsep = 3pt \fboxrule = .4pt

\newcounter{part}
\newcounter {chapter}
\newcounter {section}[chapter]
\newcounter {subsection}[section]
\newcounter {subsubsection}[subsection]
\newcounter {paragraph}[subsubsection]
\newcounter {subparagraph}[paragraph]

\def\thepart {\Roman{part}}
\def\thechapter {\arabic{chapter}}
\def\thesection {\thechapter.\arabic{section}}
\def\thesubsection {\thesection.\arabic{subsection}}
\def\thesubsubsection {\thesubsection .\arabic{subsubsection}}
\def\theparagraph {\thesubsubsection.\arabic{paragraph}}
\def\thesubparagraph {\theparagraph.\arabic{subparagraph}}

\def\@chapapp{CHAPTER}

\def\part{\cleardoublepage \thispagestyle{empty} \if@twocolumn \onecolumn
\@tempswatrue \else \@tempswafalse \fi%
\secdef\@part\@spart}

\def\@part[#1]#2{\ifnum \c@secnumdepth >-2\relax \refstepcounter{part}%
\addcontentsline{toc}{part}{\thepart \hspace{1em}#1}\else
\addcontentsline{toc}{part}{#1}\fi \markboth{}{}
\typeout{PART\space\arabic{part}.}
\chapterHead{\protect\CNsize PART \ \thepart}{#1}
\@endpart}

\def\@endpart{\newpage \if@twoside \hbox{} \thispagestyle{empty}
 \newpage
 \fi \if@tempswa \twocolumn \fi}

\def\@spart#1{{\centering \Huge \bf #1\par}\@endpart}


\def\@pnumwidth{1.55em}
\def\@tocrmarg {2.55em}
\def\@dotsep{4.5}
% don't put subsections in toc
\setcounter{tocdepth}{1}

% don't uppercase Contents
\def\tableofcontents{%
 \@restonecolfalse\if@twocolumn\@restonecoltrue\onecolumn
 \fi\chapter*{Contents\@mkboth{Contents}{Contents}}
 \@starttoc{toc}\if@restonecol\twocolumn\fi}

\def\summarycontents{%
  \begingroup
  \makeatletter
  \c@tocdepth\z@
  \let\vspace\@gobble
  \def\l@chapter{\@dottedtocline{0}{1.5em}{3em}}
  \chapter*{Summary Contents\@mkboth{Summary Contents}{Summary Contents}}
  \@input{\jobname.toc}
  \endgroup}

\def\l@part#1#2{\addpenalty{-\@highpenalty}
 \addvspace{2.25em plus 1pt} \begingroup
 \@tempdima 3em \parindent \z@ \rightskip \@pnumwidth \parfillskip
-\@pnumwidth \hskip -\leftskip
 {\large \bf \leavevmode {#1}\hfil \hbox to\@pnumwidth{\hss {#2}}}\par
 \nobreak \endgroup}

\def\l@chapter#1#2{\pagebreak[3]
 \vskip 1.0em plus 1pt \@tempdima 1.5em \begingroup
 \parindent \z@ \rightskip \@pnumwidth
 \parfillskip -\@pnumwidth
 \bf \leavevmode \advance\leftskip\@tempdima \hskip -\leftskip {#1}\nobreak\hfil
\nobreak\hbox to\@pnumwidth{\hss {#2}}\par
 \endgroup}
% Leave a bit more room for numbers
\def\l@section{\ifnum\c@tocdepth>\z@\vskip -5pt\fi\@dottedtocline{1}{1.5em}{3em}}
\def\l@subsection{\@dottedtocline{2}{3.8em}{3.2em}}
\def\l@subsubsection{\@dottedtocline{3}{7.0em}{4.1em}}
\def\l@paragraph{\@dottedtocline{4}{10em}{5em}}
\def\l@subparagraph{\@dottedtocline{5}{12em}{6em}}
% don't uppercase List Of Figures
\def\listoffigures{\@restonecolfalse\if@twocolumn\@restonecoltrue\onecolumn
 \fi\chapter*{Figures\@mkboth
 {Figures}{Figures}}\@starttoc{lof}\if@restonecol
 \twocolumn\fi}
\def\l@figure{\@dottedtocline{1}{1.5em}{3em}}
% don't uppercase List Of Tables
\def\listoftables{\@restonecolfalse\if@twocolumn\@restonecoltrue\onecolumn
 \fi\chapter*{Tables\@mkboth
 {Tables}{Tables}}\@starttoc{lot}\if@restonecol
 \twocolumn\fi}
\let\l@table\l@figure


% don't uppercase Bibliography
\def\thebibliography#1{\chapter*{Bibliography\@mkboth
 {Bibliography}{Bibliography}}\list
 {[\arabic{enumi}]}{\settowidth\labelwidth{[#1]}\leftmargin\labelwidth
 \advance\leftmargin\labelsep
 \usecounter{enumi}}
 \def\newblock{\hskip .11em plus .33em minus .07em}
 \sloppy\clubpenalty4000\widowpenalty4000
 \sfcode`\.=1000\relax}
\let\endthebibliography=\endlist

\newif\if@restonecol
\def\theindex{\typeout{INDEX}\@restonecoltrue\if@twocolumn\@restonecolfalse\fi
\columnseprule \z@
\columnsep 35pt\twocolumn[\@makeschapterhead{Index}]%
 \refstepcounter{chapter}
 \addcontentsline{toc}{chapter}{Index}
 \@mkboth{\pageBullet~~Index}{Index~~\pageBullet}%
 \parindent\z@\leftskip\z@%
 \baselineskip 11pt\ixpt{}%
 \parskip\z@ plus .3pt\relax\let\item\@idxitem}
\def\@idxitem{\par\hangindent 24pt}
\def\subitem{\par\hangindent 24pt \hspace*{12pt}}
\def\subsubitem{\par\hangindent 36pt \hspace*{24pt}}
\def\endtheindex{\if@restonecol\onecolumn\else\clearpage\fi}
\def\indexspace{\par \vskip 10pt plus 5pt minus 3pt\relax}

\def\footnoterule{\kern-3\p@ \nointerlineskip%
\moveright \baseLeftSkip \vbox{\hrule width \baseLeftSkip} \nointerlineskip%
 \kern 2.6\p@}
\@addtoreset{footnote}{chapter}
\long\def\@makefntext#1{\leftskip\baseLeftSkip \parindent 1em\noindent
 \hbox to 1.8em{\hss$^{\@thefnmark}$}#1}


\setcounter{topnumber}{2}
\def\topfraction{.7}
\setcounter{bottomnumber}{1}
\def\bottomfraction{.3}
\setcounter{totalnumber}{3}
\def\textfraction{.2}
\def\floatpagefraction{.5}
\setcounter{dbltopnumber}{2}
\def\dbltopfraction{.7}
\def\dblfloatpagefraction{.5}

\def\simpleCaption#1{\def\thefigure{\@arabic\c@figure}\caption{#1}}

\long\def\@makecaption#1#2{\vskip 6pt\hspace*{\commentWidth}\hspace*{\colGutterWidth}{\FCsize{}#1:\  #2}}
% \vskip 10pt
% \setbox\@tempboxa\hbox{#1: #2}
% \ifdim \wd\@tempboxa >\hsize #1: #2\par \else \hbox
%to\hsize{\hfil\box\@tempboxa\hfil}
% \fi}


\newcounter{figure}[chapter]
\def\thefigure{\thechapter.\@arabic\c@figure}
\def\fps@figure{tbp}
\def\ftype@figure{1}
\def\ext@figure{lof}
\def\fnum@figure{Figure \thefigure}
\def\figure{\@float{figure}}
\let\endfigure\end@float
\@namedef{figure*}{\@dblfloat{figure}}
\@namedef{endfigure*}{\end@dblfloat}
\newcounter{table}[chapter]
\def\thetable{\thechapter.\@arabic\c@table}
\def\fps@table{tbp}
\def\ftype@table{2}
\def\ext@table{lot}
\def\fnum@table{Table \thetable}
\def\table{\@float{table}}
\let\endtable\end@float
\@namedef{table*}{\@dblfloat{table}}
\@namedef{endtable*}{\end@dblfloat}

%\input titlepage.sty

\mark{{}{}}

\def\pageBullet{\hbox{$\cdot$}}

\if@twoside \def\ps@headings{\let\@mkboth\markboth%
\def\@oddfoot{\hbox{}\hfill{\RFsize\rightmark~~\thepage}}%
\def\@evenfoot{{\RFsize\thepage~~\leftmark}\hfill\hbox{}}%
\def\@evenhead{}%
\def\@oddhead{}%
\def\chaptermark##1{\markboth {\pageBullet~~##1}{}}
\def\sectionmark##1{\markright {{\ifnum \c@secnumdepth >\z@ \thesection. \ \fi ##1~~\pageBullet}}}}
\else \def\ps@headings{\let\@mkboth\markboth
\def\@oddfoot{}\def\@evenfoot{}\def\@oddhead{\hbox {}\sf \rightmark \hfil
\rm\thepage}\def\chaptermark##1{\markright {{\ifnum \c@secnumdepth
>\m@ne
 \thechapter \ \fi ##1}}}}
\fi
\def\ps@myheadings{\let\@mkboth\@gobbletwo
\def\@oddhead{\hbox{}\sf\rightmark \hfil
\rm\thepage}\def\@oddfoot{}\def\@evenhead{\rm \thepage\hfil\sf\leftmark\hbox
{}}\def\@evenfoot{}\def\chaptermark##1{}%
\def\sectionmark##1{}\def\subsectionmark##1{}}


\def\today{\ifcase\month\or
 January\or February\or March\or April\or May\or June\or
 July\or August\or September\or October\or November\or December\fi
 \space\number\day, \number\year}
\@addtoreset{equation}{chapter}
\def\theequation{\thechapter.\arabic{equation}}

% ------------------------------------------------------------------------
% from drafthead.sty
% ------------------------------------------------------------------------
%
% compute the time in hours and minutes; make new variables \timehh and \timemm
%
\newcount\timehh\newcount\timemm
\timehh=\time
\divide\timehh by 60 \timemm=\time
\count255=\timehh\multiply\count255 by -60 \advance\timemm by \count255
\def\rightnow{ \ifnum\timehh<10 0\fi\number\timehh\,:\,\ifnum\timemm<10 0
\fi\number\timemm}


\ps@plain \pagenumbering{arabic} \onecolumn \if@twoside\else\raggedbottom\fi
%
% Environments
%
\long\def\discard{\par\noindent{\hbox to \xmpWidth{\hrulefill
\ {\bf Begin Discarded Material} \ \hrulefill}} \newline}
\long\def\enddiscard{\par\noindent\hbox to \xmpWidth{\hrulefill
\ {\bf End Discarded Material} \ \hrulefill} \newline}

\newenvironment{texonly}{}{}
\newenvironment{page}[2]{\section{#2 [#1]}\label{#1}}{}
%
% Macros
%
\newcommand{\xdefault}[1]{The default value is {\tt "#1"}.}
\newcommand{\bs}{$\backslash$}
\def\noHyphens{\hyphenpenalty=10001}
\def\Autodoc{{\it The \Language{} Library Cross-Reference}}
\def\Browse{{Browse}}
\def\Clef{{Clef}}
\def\Gallery{{\Language{} Images}}
\def\HyperName{{HyperDoc}}
\def\LangName{{AXIOM}}
\def\Language{{AXIOM}}
\def\Lisp{{Common LISP}}
\def\autobuttons{}
\def\beginImportant{\vspace{.2in}\noindent\begin{boxedminipage}{\xmpWidth}%
  \def\centering{\let\\=\@centercr\rightskip\@flushglue\leftskip\@flushglue%
  \parindent\z@\parfillskip\z@}\baseLeftSkip=0pt}
\def\beginmenu{\begin{itemize}}
\def\beginscroll{}
\def\booktitle{\Language{}: The Scientific Computation System}
\def\bound#1{}
\def\downlink#1#2{{\bf #1}}
\def\endImportant{\end{boxedminipage}\vspace{.2in}}
\def\endmenu{\end{itemize}}
\def\endscroll{}
%
% following handles things like "i-th" but uses superscripts for "th"
\def\eth#1{${\hbox{#1}\:}^{\hbox{\small\rm th}}$}
%
\let\texnewline=\newline
\let\texbreak=\break
\def\free#1{}
\def\graphpaste#1{\spadcommand{#1}}
\long\def\head#1#2#3{\@nameuse{#1}{#2}\label{#3}\par}
\def\headTODO#1#2#3{\head{#1}{#2 (todo)}{#3}}
\def\helpbit#1{}
\long\def\httex#1#2{#2}
\def\indent#1{}
\def\inputbitmap#1{}
\def\mathOrSpad#1{{$#1$}}
\def\smath#1{\mathOrSpad{#1}}
\def\twodim{two-di\-men\-sion\-al}
\def\threedim{three-di\-men\-sion\-al}
\long\def\labelSpace#1{\par\vskip #1}
\def\menudownlink#1#2{\item #1 in \spadref{#2}}
\def\menuitemstyle#1{{\bf #1}}
\def\menulink#1#2{\item {\bf #1} ([#2] on page \pageref{#2}) }
\def\menulispdownlink#1#2{\item {\bf #1} }
\def\menulispmemolink#1#2{\item {\bf #1} }
\def\menumemolink{\menulink}
\def\menuspadref#1#2{\menudownlink{`#1'}{#2}}
\def\titledspadref#1#2{`#1' in \spadref{#2}}
\def\menuxmpref#1{\menudownlink{`#1'}{#1Xmp}}
\def\pp{}
\def\notequal{$\sim=$}

\def\outputMsg{\hsize=\xmpLessStepWidth\frenchspacing\Isize}  % "verbatim" lines in axiom output


\def\reftitle#1{``\@nameuse{#1Title}''}
%\def\resultType#1{{\hbox{}\hfill {\TYTsize Type:\ }\axiomType{#1}}}
\def\showBlurb#1{Issue the system command \spadcmd{)show #1} to display the full list of operations defined by \spadtype{#1}.}

\def\axiom#1{{\frenchspacing\tt #1}}
\def\spad{\axiom}

\def\Fortran{Fortran}

\def\axiomType#1{{\TYsize #1}}
\def\spadtype{\axiomType}
\def\nonLibAxiomType{\axiomType}          % things that browse can't handle
\def\pspadtype{\axiomType}

\def\spadcmd#1{{\tt #1}}
\def\spadcommand#1{\begin{flushleft}{\Isize #1}\end{flushleft}\vspace{-9pt}}
\def\spadgloss#1{\index{#1}{\it #1}}
\def\spadglossSee#1#2{\index{#2}{\it #1}}
\def\spadgraph{\spadcommand}
\def\spadkey#1{\index{#1 @\begingroup \string\tt{} #1 \endgroup}}
\def\spadpaste#1{\spadcommand{#1}}
\def\spadref#1{Section \ref{#1} on page \pageref{#1}}
\def\chapref#1{Chapter~\ref{#1}}
\def\appxref#1{Appendix~\ref{#1}}
\def\spadsig#1#2{{\sf #1 $\rightarrow$ #2}}
\def\subscriptIt#1#2{$\hbox{\it #1}_{#2}$}
\def\subscriptText#1#2{$\hbox{\it #1}_{\hbox{\small\rm #2}}$}
\def\tab#1{}
\long\def\texht#1#2{#1}
\def\void{the unique value of \exptypeindex{Void}\protect\spadtype{Void}}
\def\windowlink#1#2{{\bf #1}}
\def\xmpref#1{`\spadtype{#1}' on page \pageref{#1Xmp}}
%
% some things that are produced by tex.spad
%
\def\csch{\mathop{\rm csch}\nolimits}
\def\erf{\mathop{\rm erf}\nolimits}
\def\zag#1#2{{{\hfill \left. {#1} \right|} \over {\left| {#2} \right. \hfill}}}
%
% quick versions of following
\def\spadatt#1{{\tt #1}}
\def\spadglos#1{\index{#1}{\it #1}}
\def\spadglosSee#1#2{\index{#2}{\it #1}}
\def\spadrefUnder#1#2{\spadref{#2}}
\def\goto#1{{\bf #1}}

% the next few are used in the appendices listing categories, domains and
% packages.

\long\def\sampdata#1#2#3#4{{%
   \par\smallskip%
   {\TYsize #2}\{{\small\sf #1}\}: {\small\sf #3} {\small\it with}
   {\small\rm #4}\par}}
\long\def\condata#1#2#3#4{{%
   \par\vskip 1pt%
   {\xipt{}\TYsize #2}\allowbreak\{{\small\sf #1}\}: {\small\sf #3} {\small\it with }%
   {\small\rm #4}\par}}

\long\def\constructorListing{\par\bgroup
\baselineskip=10pt\parskip=.4\baselineskip\ixpt{}\raggedright}
\long\def\endconstructorListing{\egroup\onecolumn}

\def\appendixMark#1{\markboth{{\pageBullet~~#1}}{\@chapapp~\thechapter~~\pageBullet}}


% Bitmaps
\newcommand\ExitBitmap{{\setlength{\unitlength}{0.01in}\begin{picture}(50,16)(0,0)\special{psfile=../ps/exit.ps}\end{picture}}}

\newcommand\ReturnBitmap{{\setlength{\unitlength}{0.01in}\begin{picture}(50,16)(0,0)\special{psfile=../ps/home.ps}\end{picture}}}

\newcommand\HelpBitmap{{\setlength{\unitlength}{0.01in}\begin{picture}(50,16)(0,0)\special{psfile=../ps/help.ps}\end{picture}}}

\newcommand\UpBitmap{{\setlength{\unitlength}{0.01in}\begin{picture}(50,16)(0,0)\special{psfile=../ps/up.ps}\end{picture}}}

% file extension for library source files
\def\spadFileExt#1{.spad}

%
% Macros for function names. We need the X versions because the indexing
% software uses ! for level separators.
%

\def\fakeAxiomFun#1{{\bf #1}}              % not really in library
\def\pspadfun{\fakeAxiomFun}

\def\axiomFun#1{{\bf #1}\index{library!operations!#1 @\begingroup \string\bf{} #1 \endgroup}}
\def\spadfun{\axiomFun}

\def\axiomFunFrom#1#2{{\bf #1}\index{library!operations!#1 @\begingroup \string\bf{} #1 \endgroup}}
\def\spadfunFrom{\axiomFunFrom}

\def\axiomFunX#1{{\bf #1!}\index{library!operations!#1"! @\begingroup \string\bf{} #1"! \endgroup}}
\def\spadfunX{\axiomFunX}

\def\axiomFunFromX#1#2{{\bf #1!}\index{library!operations!#1"! @\begingroup \string\bf{} #1"! \endgroup}}
\def\spadfunFromX{\axiomFunFromX}

\def\indexAxiomSyntax#1{\index{#1 @\begingroup \string\tt{} #1 \endgroup}}
\def\axiomSyntax#1{\indexAxiomSyntax{#1}``{\tt #1}''}
\def\spadSyntax{\axiomSyntax}

\def\axiomOp#1{\index{library!operations!#1 @\begingroup \string\tt{} #1 \endgroup}``{\tt #1}''}
\def\spadop{\axiomOp}

\def\axiomOpFrom#1#2{\index{library!operations!#1 @\begingroup \string\tt{} #1 \endgroup}``{\tt #1}''}
\def\spadopFrom{\axiomOpFrom}

\def\userfun#1{\index{example functions!#1 @\begingroup \string\bf{} #1 \endgroup}{\bf #1}}

% next two used in system command descriptions
\long\def\ind{\par\noindent\hspace*{.25in}}
\long\def\unind{\par\noindent}

%
\def\exptypeindex#1{\index{library!constructors!#1 @\begingroup \string\sf{} #1 \endgroup}}
\def\syscmdindex#1{\index{system commands!#1 @\begingroup \string\tt{} )#1 \endgroup}}
%

% Some macros for Jenks' glossary
\newenvironment{items}{\begin{itemize}}{\end{itemize}}
\def\htcenter#1{\begin{center} #1 \end{center}}
\def\blankline{\par\noindent}
\def\spadsys{\spadcmd}
\def\ignore#1{\quad}
\def\spadignore#1{#1}
\def\gloss#1{{\bf #1}}
\def\glossSee#1#2{#1 (see \gloss{#2})}

% This throws away everything that is bracketed by \htonly and \endhtonly
\long\def\htonly#1\endhtonly{\endhtonly}
\def\endhtonly{}

% ------------------------------------------------------------------------
% from headerfooter.sty
% ------------------------------------------------------------------------

% Copyright (c) 1987 by Stephen Gildea
% Permission to copy all or part of this work is granted, provided
% that the copies are not made or distributed for resale, and that
% the copyright notice and this notice are retained.
%
% THIS WORK IS PROVIDED ON AN "AS IS" BASIS.  THE AUTHOR PROVIDES NO
% WARRANTY WHATSOEVER, EITHER EXPRESS OR IMPLIED, REGARDING THE WORK,
% INCLUDING WARRANTIES WITH RESPECT TO ITS MERCHANTABILITY OR FITNESS
% FOR ANY PARTICULAR PURPOSE.

% If you make any improvements, I'd like to hear about them.

% Stephen Gildea
% MIT Earth Resources Lab
% Cambridge, Mass. 02139
% mit-erl!gildea
% gildea@erl.mit.edu

% \pageheader{LEFT}{CENTER}{RIGHT}
% \pagefooter{LEFT}{CENTER}{RIGHT}

% There is no reason why these commands should not be available
% to the user.  Of course, I did fancy up the interface a bit.
% By mit-erl!gildea 11 October 1986
% minor changes 14 Oct 87 gildea
% added \pageheaderlinetrue feature 9 Dec 88 gildea

% All of these commands take three arguments, which are printed at
% the left, center, and right of each page.  All three args must be
% provided even if some of them are empty.  The odd and even
% variations are only useful if you are using the twoside option.

% Example: \pagefooter{}{\thepage}{}

% Say \pageheaderlinetrue if you want the header underlined.

\newif\ifpageheaderline \pageheaderlinefalse

\newdimen\pageheaderlineskip  \pageheaderlineskip=1.5pt

\def\@headerline#1#2#3{%
        \ifpageheaderline
          \vbox{\hbox to \textwidth{\rlap{#1}\hss{#2}\hss\llap{#3}}%
          \vskip\pageheaderlineskip \hrule}%
        \else{\rlap{#1}\hss{#2}\hss\llap{#3}}\fi}

\newcommand\oddpageheader[3]{\def\@oddhead{\@headerline{#1}{#2}{#3}}}

\newcommand\evenpageheader[3]{\def\@evenhead{\@headerline{#1}{#2}{#3}}}

\newcommand\oddpagefooter[3]{\def\@oddfoot{\rlap{#1}\hss{#2}\hss\llap{#3}}}

\newcommand\evenpagefooter[3]{\def\@evenfoot{\rlap{#1}\hss{#2}\hss\llap{#3}}}

\newcommand\pageheader[3]{%
        \oddpageheader{#1}{#2}{#3}\evenpageheader{#1}{#2}{#3}}

\newcommand\pagefooter[3]{%
        \oddpagefooter{#1}{#2}{#3}\evenpagefooter{#1}{#2}{#3}}

% ------------------------------------------------------------------------
% from drafthead.sty
% ------------------------------------------------------------------------
%
% compute the time in hours and minutes; make new variables \timehh and \timemm
%
\newcount\timehh\newcount\timemm
\timehh=\time
\divide\timehh by 60 \timemm=\time
\count255=\timehh\multiply\count255 by -60 \advance\timemm by \count255
\def\rightnow{ \ifnum\timehh<10 0\fi\number\timehh\,:\,\ifnum\timemm<10 0
\fi\number\timemm}

% ------------------------------------------------------------------------
% the definition of boxedminipage
% ------------------------------------------------------------------------

%   boxedminipage.sty
%
% adds the boxedminipage environment---just like minipage, but has a
% box round it!
%
% The thickneess of the rules around the box is controlled by
% \fboxrule, and the distance between the rules and the edges of the
% inner box is governed by \fboxsep.
%
% This code is based on Lamport's minipage code.
%
% Fixed, 7 Jun 89 by Jerry Leichter
%   Leave \fboxsep worth of separation at top and bottom, not just at
%   the sides!
%
\def\boxedminipage{\@ifnextchar [{\@iboxedminipage}{\@iboxedminipage[c]}}

\def\@iboxedminipage[#1]#2{\leavevmode \@pboxswfalse
  \if #1b\vbox
    \else \if #1t\vtop
         \else \ifmmode \vcenter
                \else \@pboxswtrue $\vcenter
             \fi
      \fi
  \fi\bgroup % start of outermost vbox/vtop/vcenter
    \hsize #2
    \hrule\@height\fboxrule
    \hbox\bgroup % inner hbox
      \vrule\@width\fboxrule \hskip\fboxsep \vbox\bgroup % innermost vbox
    \vskip\fboxsep
    \advance\hsize -2\fboxrule \advance\hsize-2\fboxsep
    \textwidth\hsize \columnwidth\hsize
    \@parboxrestore
    \def\@mpfn{mpfootnote}\def\thempfn{\thempfootnote}\c@mpfootnote\z@
    \let\@footnotetext\@mpfootnotetext
    \let\@listdepth\@mplistdepth \@mplistdepth\z@
    \@minipagerestore\@minipagetrue
    \everypar{\global\@minipagefalse\everypar{}}}

\def\endboxedminipage{%
    \par\vskip-\lastskip
    \ifvoid\@mpfootins\else
      \vskip\skip\@mpfootins\footnoterule\unvbox\@mpfootins\fi
    \vskip\fboxsep
      \egroup % ends the innermost \vbox
      \hskip\fboxsep \vrule\@width\fboxrule
    \egroup % ends the \hbox
    \hrule\@height\fboxrule
  \egroup% ends the vbox/vtop/vcenter
  \if@pboxsw $\fi}


% This redefinition of \cleardoublepage forces the headers and
% footers to be omitted on otherwise blank pages.
\def\cleardoublepage{\clearpage\if@twoside \ifodd\c@page\else
    \thispagestyle{empty}
    \hbox{}\newpage\if@twocolumn\hbox{}\newpage\fi\fi\fi}

\def\gotoevenpage{\clearpage\if@twoside\ifodd\c@page%
    \hbox{}\newpage\if@twocolumn\hbox{}\newpage\fi\fi\fi}

% This redefinition of the enumerate environment tightens up
% the space between items.

\def\enumerate{\ifnum \@enumdepth >3 \@toodeep\else
      \advance\@enumdepth \@ne
      \edef\@enumctr{enum\romannumeral\the\@enumdepth}\list
      {\csname label\@enumctr\endcsname}{\parsep \z@%
       \if@twocolumn\else\linewidth\xmpWidth\fi \itemsep 2pt \topsep 0pt
        \usecounter{\@enumctr}\def\makelabel##1{\hss\llap{##1}}}\fi}

% This redefinition of the itemize environment tightens up
% the space between items.

\def\itemize{\ifnum \@itemdepth >3 \@toodeep\else \advance\@itemdepth \@ne
\edef\@itemitem{labelitem\romannumeral\the\@itemdepth}%
\list{\csname\@itemitem\endcsname}{%\rightmargin 0pt
 \if@twocolumn\else\linewidth\xmpWidth\fi \parsep\z@%
 \itemsep 2pt \topsep 0pt\def\makelabel##1{\hss\llap{##1}}}\fi}

% This redefinition of the description environment
% accounts for the increased left margin

\def\descriptionlabel#1{\hspace\labelsep \bf #1}
\def\description{\list{}{\labelwidth\z@ \itemindent-\leftmargin
 \if@twocolumn\else\linewidth\xmpWidth\fi%
 \itemsep=2pt \parsep\z@
 \let\makelabel\descriptionlabel}}
\let\enddescription\endlist
\newdimen\descriptionmargin
\descriptionmargin=0pt

% This redefinition of \centering accounts for the increased left margin
% It has to be redefined within minipages so that the \leftmargin is
% set back to 0pt

\def\centering{\let\\=\@centercr\rightskip\@flushglue\leftskip\@flushglue
\advance\leftskip by \baseLeftSkip \parindent\z@\parfillskip\z@}

% This environment provides a simple, unbulleted list, one level deep.

\def\simpleList{%
%\begin{list}{}{\itemsep 2pt \topsep 2pt \parsep \z@
\begin{list}{}{\itemsep 2pt \topsep \parskip \parsep \z@
\leftmargin 1pc \partopsep \z@ \parskip \z@
\if@twocolumn\else\linewidth\xmpWidth\fi}}
\def\endsimpleList{\end{list}\vskip -\parskip}

% something so that spadsrc environments at least print

\def\spadsrc{\@ifnextchar [{\@spadsrc}{\@spadsrc[0]}}

\long\def\@spadsrc[#1]{\begingroup\Isize}%
\def\endspadsrc{\endgroup}%

% the redefinition for verbatim uses the small \tt font

\def\@verbatim{\trivlist \item[]%
\leftskip\baseLeftSkip
\rightskip\z@\topsep 0pt\partopsep 0pt
\parindent\z@\parfillskip\@flushglue\parskip\z@
\@tempswafalse \def\par{\if@tempswa\hbox{}\fi\@tempswatrue\@@par}
\obeylines\Isize\catcode``=13 \@noligs \let\do\@makeother \dospecials}

\def\newendverbatim{\endtrivlist\vskip -\lastskip}
\let\endverbatim=\newendverbatim

% this is a verbatim mode with the tiny font
\begingroup \catcode `|=0 \catcode `[= 1
\catcode`]=2 \catcode `\{=12 \catcode `\}=12
\catcode`\\=12 |gdef|@xtinyverbatim#1\end{tinyverbatim}[#1|end[tinyverbatim]]
|gdef|@sxtinyverbatim#1\end{tinyverbatim*}[#1|end[tinyverbatim*]]
|endgroup

\def\@stinyverbatim{\obeyspaces\@tinyverbatim}

\def\@tinyverbatim{\trivlist \item[]%\if@minipage\else\vskip\parskip\fi%
\leftskip\baseLeftSkip
\rightskip\z@\topsep 0pt\partopsep 0pt
\parindent\z@\parfillskip\@flushglue\parskip\z@
\@tempswafalse \def\par{\if@tempswa\hbox{}\fi\@tempswatrue\@@par}
\obeylines \tiny\tt\baselineskip 8pt \catcode``=13 \@noligs \let\do\@makeother \dospecials}

\def\tinyverbatim{\@tinyverbatim \frenchspacing\@vobeyspaces \@xtinyverbatim}
\def\newendtinyverbatim{\endtrivlist\vskip -\lastskip}
\let\endtinyverbatim=\newendtinyverbatim

% ------------------------------------------------------------------------
% the code, comments, line numbers environment
% ------------------------------------------------------------------------

\long\def\horizontalline{%
  \par \nointerlineskip%
  \moveright \baseLeftSkip \vbox{\hrule height .5pt width \xmpWidth}%
  \nointerlineskip\par}

\newcount\xmpLineCount \xmpLineCount=0

%% This one does not reset the line numbers. Does have horizontal rules.

\def\xmpLinesNoReset{\par\noindent\vskip .5\baselineskip\horizontalline%
\vskip .25\baselineskip\bgroup\obeyspaces\parskip\z@ \baselineskip=10pt}
\def\endxmpLinesNoReset{\egroup\vskip .25\baselineskip\horizontalline\vskip .5\baselineskip}

%% This one does reset the line numbers. Does have horizontal rules.

\def\xmpLines{\global\xmpLineCount=0\xmpLinesNoReset}
\let\endxmpLines=\endxmpLinesNoReset

%% This one does not reset the line numbers. Does not have horizontal rules.

\def\xmpLinesNoResetPlain{\par\noindent\bgroup\obeyspaces\parskip\z@ \baselineskip=10pt}
\def\endxmpLinesNoResetPlain{\egroup\par}

%% This one does reset the line numbers. Does not have horizontal rules.

\def\xmpLinesPlain{\global\xmpLineCount=0\xmpLinesNoResetPlain}
\let\endxmpLinesPlain=\endxmpLinesNoResetPlain

\long\def\xmpLine#1#2{%
% #1 = line with special characters escaped, #2 = comment
\par%\vskip -.5\baselineskip
\global\advance\xmpLineCount by 1%
\hspace*{-\baseLeftSkip}%
\begin{minipage}[t]{\commentWidth}\raggedright\COMsize\hspace*{0pt}{#2}\end{minipage}%
\hspace{\colGutterWidth}%
\hbox{\noHyphens\frenchspacing\Isize{#1}}\hfill%
\hbox{\small$\number\xmpLineCount$}%
\par}%

%\def\topfigrule{\moveright \baseLeftSkip \vbox{\hrule width \xmpWidth}}
%\def\botfigrule{\moveright \baseLeftSkip \vbox{\hrule width \xmpWidth}}

\def\figXmpLines{%
\begin{figure}[tbh]
\xmpLineCount=0 \vskip .5\baselineskip%\horizontalline%
\baseLeftSkip\z@%
\vskip .25\baselineskip\bgroup\obeyspaces\baselineskip=10pt}
\def\endfigXmpLines{\egroup\vskip .25\baselineskip
\baseLeftSkip=\savedBaseLeftSkip%
%\horizontalline
\vskip .5\baselineskip\end{figure}}

% ------------------------------------------------------------------------
% the two-column comment and \spadcommand macros
% ------------------------------------------------------------------------

% \inleftmar pushes its argument into the left margin, leaving a little
% space.
\def\inleftmar#1{{\setbox0=\hbox{#1\ }\hspace*{-\wd0}\copy0}}

% redefine the display math macros

\def\[{\relax\hbox\bgroup \DMsize\openup1\jot$ \displaystyle \bgroup}
\def\]{\relax\egroup $\egroup}

\def\narrowDisplay#1{\par\vskip -\parskip\vskip\abovedisplayskip\noindent%
{\hsize=\xmpWidth\centerline{$\displaystyle{#1}$}}%
\par\vskip -\parskip\vskip\belowdisplayskip}

\newbox\spadOutput      % box to contain spad output
\newbox\spadStepNum     % box to contain spad step number
\newbox\spadResult      % box to contain spad output result type
\newbox\spadTypeBox     % box that contains Type: - for measurement
\setbox\spadTypeBox=\hbox{{\TYTsize Type:\ }}

\def\@blsp{
}
\def\formatStepNum#1{\edef\@steptmp{#1}%
    \ifx\@steptmp\@empty \setbox\spadStepNum=\vbox{\hbox{}}%
    \else\setbox\spadStepNum=\vbox{\hbox{\@normalsize\rm({#1})}}\fi}

\def\formatResultType#1{\edef\@restmp{#1}%
   \ifx\@restmp\@empty \setbox\spadResult=\hbox to 0pt{\kern 0pt}%
   \else\setbox\spadResult=\hbox{{\TYTsize Type:\ }{\TYsize #1}}
     \ifdim\wd\spadResult > \hsize% type is too big to fit on a line
        \setbox\spadResult=\hbox{\begin{minipage}[t]{\textwidth}%
          \raggedright\hangafter=1\hangindent=\wd\spadTypeBox
          \advance\hangindent by 2pt %
          {\TYTsize Type:\ }{\TYsize #1}\end{minipage}}
   \fi\fi}

\long\def\xtc#1#2{% #1 = comment text, #2 = input line
  \par \leftskip=0pt \vskip -\parskip \vskip .5\baselineskip\noindent%
  {\raggedright\hbadness = 10001%
  \def\spadcommand##1{{\hangafter=1 \hangindent=1pc \frenchspacing%
      \ttraggedright\noHyphens\Isize ##1\par\unskip}}%
  \begin{minipage}[t]{\commentWidth}
  \raggedright\COMsize{#1}
  \end{minipage}\hfill%
  \begin{minipage}[t]{\xmpWidth}
  \raggedright\Isize{#2}
  \end{minipage}}%
 \par\leftskip=\baseLeftSkip%
}

\long\def\xtcLong#1#2#3#4#5{%
% #1 = comment text, #2 = input line, #3 = math output, #4 = step number,
% #5 = result type
  \par \leftskip=0pt%
  \vskip -\parskip \vskip .5\baselineskip\noindent%
  {\raggedright \hbadness = 10001%
  \def\spadcommand##1{{\hangafter=1 \hangindent=1pc \frenchspacing%
      \ttraggedright\noHyphens\Isize ##1\par\unskip}}%
  \begin{minipage}[t]{\commentWidth}
  \raggedright\COMsize{}{#1}
  \end{minipage}\hfill%
  \dimen3=\xmpWidth
  \begin{minipage}[t]{\dimen3}
  \begin{minipage}[t]{\dimen3}%
  \raggedright\Isize{#2}%
  \end{minipage}%
  \edef\@restmp{#5}
  \vskip .5\baselineskip
  \setbox\spadOutput=#3
  \formatStepNum{#4}
  \vbox{\hbox to \xmpWidth{\box\spadOutput\hfill\box\spadStepNum}}
  % if type is empty don't do any fancy formatting for it
  \ifx\@restmp\@blsp
  \else
    \formatResultType{#5}
    \vskip 3pt%
    \vbox{\hbox to \xmpWidth{\hbox{}\hfill\box\spadResult}}
  \fi
  \end{minipage}}%
 \par\leftskip=\baseLeftSkip%
}

% This is meant for things (usually system commands) that produce
% absolutely no output. It is renamed because \xtc forms get changed
% to \xtcLong forms to display the output.
\let\noOutputXtc=\xtc

% This is for things that should appear as if they are two-column
% input/output but we are reaaly only showing the input.
\let\nullXtc=\xtc

% this should be displayed like a spad command but should not be processed
% by awk scripts as one.
\long\def\nullspadcommand{\spadcommand}

% next line must not have closing brace at end
\long\def\psXtc#1#2#3{{\xtcLong{#1}{#2}{\vbox{\epsfxsize=.61\textwidth{#3}}}{}{
}}}

\def\epsfsize#1#2{.24\textheight}

\pagestyle{headings}
\usepackage{makeidx}
\makeindex
