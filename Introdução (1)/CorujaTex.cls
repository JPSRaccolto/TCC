%	                      ¦@@@@@@@@@@@@@@%                      
%	         ¦@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@          
%	              @@@@@@@@@@@ CorujaTeX @@@@@@@@@@               
%	                   @@@@@@@@@ by @@@@@@@@@                   
%	        @@@@@@       @@@@ Ademário @@@@       @@@@@         
%	        @@@@@           @@@@@@@@@@@           @@@@@@        
%	        @@@@  @@@@@       @@@@@@@=      @@@@@   @@@         
%	        #@@   @@@@@@       @@@@@     !  @@@@@@  @@@         
%	          @@   @@@   @@@    @@@     @@   @@@   @@           
%	            @@     @@@@@@    @     @@@@@     @@             
%	                @@@@@@@             @@@@@@@                 
%	          @@@                                 @@@           
%	          @@@@@      C@ @@@     @@@ @@      @@@@@           
%	          @@@@@     @@ @ 5@@   @@  @ @@ #   @@@@@           
%	          $@@@@    @@@@@@@@@@  @@@@@@@@@@   @@@@@           
%	           @@@@@   @@@@@3@@@@ @@@@@3@@@@@   @@@@            
%	           @@@@@    @@‘ @ @@ @ 3@@ @ @@    @@@@@            
%	            @@@@@   @@@@@@@@@@@@@@@@@@@@   @@@@             
%	            @@@@@@   @@@@ @@@@@@@@ @@@@   @@@@@             
%	             @@@@@#   @@@@@@@@@@@@@@@@   @@@@@              
%	              @@@@@@   @@@ @@@@@@ @@@   @@@@@               
%	               =@@@@@    @@@@@@@@@@   @@@@@J                
%	                 @@@@@@     @@@@     @@@@@                  
%	                   @@@@@@    @    @@@@@@                    
%	        @@@@@        @@@@@@     @@@@@@                      
%	      @@@@@@@@         @@@@@@@@@@@@@                        
%	    @@@@@@@@@@@            @@@@@@                           
%	 @@@@@@@@    @@                                             
%	@@@@@       @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
%	@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 
%	@@@@@@@@     @@@                   @              @@@@@@    
%	  @@@@@@@@ @@@@       @@@ @@ @@ @@ @@                       
%	      @@@@@@@@@      @@@ @@  @@ @@@ @@@                     
%	        @@@@@      @@@@ @@@ @@@  @@@ @@@                    
%	                  @@@@ @@@@ @@@  @@@  @@@@                  
%	                    @ @@@@  @@@@ @@@@                       
%	                         @  @@@@                            

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{CorujaTex}[28/12/2017 - 08/04/2021 - LaTeX Class by Ademario]

%\newcommand{\headlinecolor}{\normalcolor}


\DeclareOption{tcc}{% Trabalho de Conlusão de Curso - monografia
	\let\@optrabalho=0}

\DeclareOption{ptcc}{% Pré TCC - Proposta de monografia
	\let\@optrabalho=1}

\DeclareOption{dis}{% dissetação de mestrado
	\let\@optrabalho=2}

\DeclareOption{pdis}{% dissetação de mestrado
	\let\@optrabalho=3}

\DeclareOption{qdoc}{% qualificação de doutorado
	\let\@optrabalho=4}

\DeclareOption{doc}{% tese de doutorado
	\let\@optrabalho=5}

\DeclareOption{sty0}{% estilo de apresentação dos capítulos
	\let\@opestilo=0}

\DeclareOption{sty1}{% estilo de apresentação dos capítulos
	\let\@opestilo=1}

\DeclareOption{sty2}{% estilo de apresentação dos capítulos
	\let\@opestilo=2}

\DeclareOption{sty3}{% estilo de apresentação dos capítulos
	\let\@opestilo=3}

\DeclareOption{sty4}{% estilo de apresentação dos capítulos
	\let\@opestilo=4}

\DeclareOption{sty5}{% estilo de apresentação dos capítulos
	\let\@opestilo=5}

\DeclareOption{sty6}{% estilo de apresentação dos capítulos
	\let\@opestilo=6}

\DeclareOption{digital}{%
	\PassOptionsToClass{oneside}{book}}

\DeclareOption{impresso}{%
	\PassOptionsToClass{twoside}{book}}

\ExecuteOptions{tcc,sty4}
\ProcessOptions

%\LoadClass[12pt,a4paper,twoside]{book}
%\LoadClass[12pt,a4paper,oneside,openright]{book}
\LoadClass[12pt,a4paper]{book}

\RequirePackage{hyperref} % precisa ficar no início das chamadas dos pacotes...evita erros
\hypersetup{
	colorlinks=true,
	linkcolor=black,  %blue,
	filecolor= black, %magenta,      
	urlcolor=blue, %cyan,
	pdftitle={CorujaTeX - UFOB - CCEE},
	pdfpagemode=UseOutlines, %Determina como o arquivo é aberto. As possibilidades são UseThumbs (Miniaturas), UseOutlines (Marcadores) e FullScreen.
	citecolor=black,%green
}

%\RequirePackage{xcolor}
\RequirePackage[brazil]{babel}
\RequirePackage{graphicx}
\RequirePackage[top=3cm, bottom=2cm, left=3cm, right=2cm]{geometry}
\RequirePackage[T1]{fontenc}
\RequirePackage[utf8]{inputenc}
\RequirePackage{ifthen}
\RequirePackage{lipsum}
\RequirePackage[explicit]{titlesec}
\RequirePackage{tikz}
\RequirePackage{lmodern} % Importante para o estilo 3 - sty3 
\RequirePackage{lastpage}
\RequirePackage{xspace} 
\RequirePackage{fix-cm}
\RequirePackage{url}
\RequirePackage{amsmath,amsfonts,amssymb,amsthm}
\RequirePackage{textcase}
\RequirePackage{textcomp}
\RequirePackage{epigraph}
\RequirePackage[alf]{abntex2cite}	% Citações padrão ABNT
\RequirePackage{quoting} % utilizado para citacao direta
\RequirePackage{booktabs, xcolor, colortbl, rotating, multirow, bigstrut} % tabelas (Excel2LaTeX)

\RequirePackage[printonlyused]{acronym} % Acrônimos

\RequirePackage{subfigure}
\RequirePackage[final]{pdfpages}
\RequirePackage{quoting}
\RequirePackage[outdir=./]{epstopdf}
\RequirePackage{comment}


%\acrodef{EX}[EX]{\emph{Example}}
%\RequirePackage{csquotes} %voltar a mexer

\usepackage{listings} % usado quando for inserir códigos fonte de programas
	\lstset{numberbychapter=false} %numeração sequencial 
	\gdef\lstlistingname{Programa}
	\gdef\lstlistlistingname{Lista de Programas}

\usepackage{indentfirst}% Indenta o primeiro parágrafo de cada seção.
	\setlength{\parindent}{1.25cm}
	\setlength{\parskip}{6pt}

\RequirePackage{color}
\RequirePackage{caption} 

	\DeclareCaptionFont{red}{\color{red}}
	\DeclareCaptionFont{blue}{\color{blue}}
	\captionsetup{labelsep=endash} % aplica em todos ambientes
	%\captionsetup[figure]{labelfont=bf,textfont=it}
	\captionsetup[figure]{labelfont={bf}}
	\captionsetup[table]{labelfont=bf}
	\captionsetup[quadro]{labelfont=bf}
	%\captionsetup[programa]{labelfont=bf}

\RequirePackage{fancyhdr} % Onde está definito o estilo do cabeçalho - cpmo a linha horizontal, por exemplo
	\fancyhf{} % importante para que os números da páginas no rodapá não apareceça (exceto na primeira ágina do capítulo)
	\pagestyle{empty} 
	%\pagestyle{headings}
	%\pagestyle{fancy} 
	%\fancyhead{}
	%\patchcmd{\chapter}{\if@openright\cleardoublepage\else\clearpage\fi}{}{}{}
	\fancyhead[LO]{\footnotesize\sffamily\ifthenelse{\isodd{\value{page}}}{\leftmark}{\rightmark}}
	\fancyhead[RO]{\footnotesize\sffamily\thepage}
	\fancyhead[RE]{\footnotesize\sffamily\ifthenelse{\isodd{\value{page}}}{\leftmark}{\rightmark}}
	\fancyhead[LE]{\footnotesize\sffamily\thepage}

%\fancyfoot[CE]{\leftmark{ }\rightmark}
%\renewcommand{\headrulewidth}{0.5pt}
\RequirePackage{newfloat}  
%\newlistof{quadro}{lod}{Lista de Quadros}
\DeclareFloatingEnvironment[
	fileext=lod, % só aparece na lista se colocar lod (loq não funciona)
	placement={!hbtp},
	name=Quadro,
	within=chapter,
	listname={Lista de quadros} % Título da página da lista
]{quadro}


\DeclareFloatingEnvironment[
	fileext=lod, % só aparece na lista se colocar lod (loq não funciona)
	placement={!hbtp},
	name=Program,
	within=chapter,
	listname={Lista de Program} % Título da página da lista
]{program}


%\usepackage[texcoord,grid,gridunit=mm,gridcolor=black!30,subgridcolor=gray!10]{eso-pic}

\newcommand{\aasp}{``} % abre aspas duplas
\newcommand{\fasp}{''} % fecha aspas duplas

\counterwithout{equation}{chapter} %numeração sequencial no ambiente equation
%\def\equationautorefname~#1\null{%
%	Equação~(#1)\null }
\counterwithout{quadro}{chapter} %numeração sequencial no ambiente quadro
%\def\equationautorefname~#1\null{%
%	Quadro~(#1)\null }
\counterwithout{figure}{chapter} %numeração sequencial no ambiente figure
\counterwithout{table}{chapter}%numeração sequencial no ambiente table


%\counterwithout{lstlisting}{chapter}%numeração sequencial no ambiente table

\addto\captionsbrazil{
	\renewcommand{\subsubsectionautorefname}{subse{\c c}\~ao}
	\renewcommand{\figureautorefname}{Figura}
	\renewcommand{\tableautorefname}{Tabela}
	%\renewcommand{\figurename}{Fig.}
}

%\addto\captionsenglish{
%	\renewcommand{\figurename}{Fig.}
%}

%\definecolor{slcolor}{HTML}{882B21}
%
%
%
%\DeclareOption{onecolumn}{\OptionNotUsed}
%\DeclareOption{green}{\renewcommand{\headlinecolor}{\color{green}}}
%\DeclareOption{red}{\renewcommand{\headlinecolor}{\color{slcolor}}}
%\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
%\ProcessOptions\relax

%%% Comandos originais do sharelatex...não pretendo utilizar nada desse tipo
%%%%%%\newcommand{\titulo}{%
%%%%%%	\twocolumn[%
%%%%%%	\fontsize{20}{60}\fontfamily{phv}\fontseries{b}%
%%%%%%	\fontshape{sl}\selectfont\headlinecolor
%%%%%%	\@title
%%%%%%	
%%%%%%	\medskip
%%%%%%	]%
%%%%%%}
%%%%%%
%%%%%%\renewcommand{\section}{%
%%%%%%	\@startsection
%%%%%%	{section}{1}{0pt}{-1.5ex plus -1ex minus -.2ex}%
%%%%%%	{1ex plus .2ex}{\large\sffamily\slshape\headlinecolor}%
%%%%%%}
%%%%%%
%%%%%%\renewcommand{\normalsize}{\fontsize{9}{10}\selectfont}
%%%%%%\setlength{\textwidth}{17.5cm}
%%%%%%\setlength{\textheight}{22cm}
%%%%%%\setcounter{secnumdepth}{0}


% \gdef\@masculinorrtitulo{Orientador}
% \gdef\@femininoorientadortitulo{Orientadora}
%\gdef\@malecoadvisertitle{Coorientador}
%\gdef\@femalecoadvisertitle{Coorientadora}
\gdef\@monografia{Monografia de Gradua\c{c}\~{a}o}
\gdef\@propostamonografia{Proposta para Trabalho de Gradua\c{c}\~{a}o}
\gdef\@dissertacao{Disserta\c{c}\~{a}o de Mestrado}
\gdef\@aspasduplasabrir{``}

%\gdef\@corujatex{Coruja\TeX}
\gdef\corujatex{Coruja\TeX{ }}

% CorujaTeX name
%\newcommand{\corujatex}{Coruja\TeX}
%\newcommand{\CorujaTex}{Coruja\TeX}
%\newcommand{\CorujaTeX}{Coruja\TeX}



\gdef\@colophontext{%
	\centering
	\textsf{\corujatex}\\
	\includegraphics[width=1.2cm]{./__suporte/logo_marca}\\
	Este volume foi tipografado em \LaTeX\ na classe \textsf{Coruja\TeX} como uma demanda do Colegiado do curso de Engenharia Elétrica da \acf*{UFOB}\\
	\urlstyle{sf}%
	(\url{https://github.com/ademariocarvalho/CCEE_UFOB_CorujaTEX}).
	%Para detalhes sobre este documento, clique \Acrobatmenu{GeneralInfo}{aqui}.
}

\def\colophon{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\null\vfill
	\small\noindent\@colophontext
}


%ambiente para citação direta... retirado e adaptado da abntex2 
\newenvironment*{citacaodireta}[1][default]{%
	\list{}%
	\footnotesize%
	\addtolength{\leftskip}{3cm}% Ficou desajustado com 4cm ...3cm foi o que gerou 4 cm na impressão
	\item[]%
	\singlespace
	%\begin{SingleSpace}%
	%	\ifthenelse{\not\equal{#1}{default}}{\itshape\selectlanguage{#1}}{}%
	}{%
	%\end{SingleSpace}%
	\endlist
	
}%

\newenvironment*{citacaodireta2}{
	\begin{quoting}[rightmargin=0cm,leftmargin= 4 cm]
		\begin{singlespace}
			\footnotesize \noindent}{
		\end{singlespace}
\end{quoting}}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% IDENTIFICAÇÃO
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\def\autoria#1{%
	{\footnotesize Fonte: #1}}

\def\instituicao#1{%
	\gdef\@instituicao{#1}}
\def\@instituicao{Universidade Federal do Oeste da Bahia}

\def\brasao#1{%
	\gdef\@brasao{#1}}
\def\@brasao{\includegraphics[width=2.61cm]{./__suporte/logo_brasao}}


\def\curso#1{%
	\gdef\@curso{#1}}
\def\@curso{Engenharia Elétrica} %% 

\let\@campusmostrar=0
\def\campus#1{%
	\let\@campusmostrar=1
	\gdef\@campus{#1}}
%\campus{Centro Multidisciplinar de Bom Jesus da Lapa}

\def\setor#1{%
	\gdef\@setor{#1}}
\def\@setor{Colegiado de Engenharia Elétrica}

\def\cidade#1{%
	\gdef\@cidade{#1}}
\def\@cidade{Bom Jesus da Lapa--BA}

\def\titulo#1{%
	\gdef\@titulo{#1}}
\def\@titulo{Por favor, informe o título do seu trabalho...}

\def\subtitulo#1{%
	\gdef\@subtitulo{#1}}
\def\@subtitulo{ }

\def\autor#1{%
	\gdef\@autor{#1}}
\def\@autor{você não informou o autor do trabalho}

%\def\orientador#1{%
%	\gdef\@orientador{#1}}
%\def\@orientador{informe o nome do seu orientador}

\let\@orientadormostrar=0
\def\orientador#1{%
	\let\@orientadormostrar=1
	\gdef\@orientador{#1}}

\let\@orientadoramostrar=0
\def\orientadora#1{%
	\let\@orientadoramostrar=1
	\gdef\@orientadora{#1}}

\let\@coorientadormostrar=0
\def\coorientador#1{%
	\let\@coorientadormostrar=1
	\gdef\@coorientador{#1}}

\let\@coorientadoramostrar=0
\def\coorientadora#1{%
	\let\@coorientadoramostrar=1
	\gdef\@coorientadora{#1}}


\gdef\@textoapresentacao{%
	\if\@optrabalho0{%
		\textit{Trabalho de conclus\~{a}o de curso apresentado ao \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao, como requisito parcial para obten\c{c}\~{a}o do grau de Bacharel em \@curso.} }	
	\fi
	
	\if\@optrabalho1{%
		\textit{Esta proposta de trabalho de conclus\~{a}o de curso apresentada ao \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao, como requisito parcial para obten\c{c}\~{a}o do grau de Bacharel em \@curso.} }	
	\fi
	
	\if\@optrabalho2{%
		\textit{Disserta\c{c}\~{a}o de Mestrado apresentada ao \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao, como requisito parcial para obten\c{c}\~{a}o do título de Mestre em \@curso.} }	
	\fi
}%

\gdef\@textoaprovacao{%
	\if\@optrabalho0{%
		\sf {Esta monografia foi aprovada como requisito parcial para obtenção do título de Bacharel em \@curso, aprovada em sua forma final pelo \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao.}}
	\fi
	
	\if\@optrabalho1{%
		\sf {Esta proposta de trabalho de conclusão de curso foi aprovada como requisito parcial para obtenção do título de Bacharel em \@curso, aprovada em sua forma final pelo \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao.}}
	\fi
	
	\if\@optrabalho2{%
		\sf {Esta Disserta\c{c}\~{a}o foi aprovada como requisito parcial para obtenção do título de Mestre em \@curso, aprovada em sua forma final pelo \@setor\ \if\@campusmostrar1 do \@campus\ \fi da \@instituicao.}}
	\fi
}%

% Esta disserta\c{c}\~{a}o foi aprovada como requisito parcial \`{a} obten\c{c}\~{a}o do t\'{i}tulo de Mestre em Engenharia El\'{e}trica e aprovada em sua forma final pelo Programa de P\'{o}s-Gradua\c{c}\~{a}o em Engenharia El\'{e}trica da Universidade Federal da Bahia.



\def\@tipodetexto{%
	\if\@optrabalho0
		\@monografia
	\else
	\if\@optrabalho1
		\@propostamonografia
	\else
	\if\@optrabalho2
		\@dissertacao
	\fi\fi\fi}%



%\def\cutter#1{%
%	\gdef\@cutter{#1}}

\def\nomeif#1{%
	\ifx&#1&%
		\gdef\@intx{Masculino}
	\else
		\gdef\@intx{Feminino}
	\fi    
}%

%\newcommand\test[1]{%
%	\ifx&#1&%
%		\gdef\@intx{Masculino}
%	\else
%		\gdef\@intx{Feminino}
%	\fi}

%\newcommand\test[1]{%
%	\ifthenelse{\equal{#1}{}}{true}{false}}


\newcommand\Orientador[2][M]
{%
	\ifthenelse{\equal{#1}{F}}
	{%if -> true
		\gdef\@orientadortitulo{Orientadora} % feminino
	}
	{%else -> false
		\gdef\@orientadortitulo{Orientador}  % masculino
	}
	\gdef\@orientadornome{#2}  % masculino
}%

%\uppercase{text}
%\lowercase{text}
%\MakeUppercase{text}
%\MakeLowercase{text}

%\let\TextBF\textbf%
\let\imprimir\include%

\def\printFolhaAprovacao{
	\include{___pretextual/folha_de_aprovacao}
}

\def\printDedicatoria{
	\if\@optrabalho0
		\iniciodedicatoria
			\include{___pretextual/dedicatoria}
		\finaldedicatoria
	\fi
	
	\if\@optrabalho2
		\iniciodedicatoria
			\include{___pretextual/dedicatoria}
		\finaldedicatoria
	\fi
}


\def\printFiguras{
	\listoffigures	% lista de figuras
	\thispagestyle{empty}
}

\def\printTabelas{
	\listoftables   % lista de tabelas
	\thispagestyle{empty}
}

\def\printQuadros{
	\listofquadro   % lista de tabelas
	\thispagestyle{empty}
}

\def\printAcronimos{
	\include{___pretextual/lista_AAS} % inserir lista de abreviaturas, acrônimos e siglas
}

\def\printSimbolos{
	\include{___pretextual/lista_simbolos}
}	

\def\printSumario{
	\tableofcontents % sumário
	\thispagestyle{empty}
	\newpage
	\thispagestyle{empty}
	\pagestyle{fancy}
	\onehalfspace
}	


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CAPA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\def\printCapa{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\begin{center}
		\@brasao\\ %\includegraphics[width=2.61cm]{logo.png}
		\sf\normalsize\MakeUppercase{\@instituicao}\\
		\if\@campusmostrar1
		\@campus\\
		\fi
		\textbf{\@setor } %\@curso
		\vskip 42mm
		\MakeUppercase{\@autor}
		\vskip 40mm
		\Large \MakeUppercase{\textbf{\@titulo}}\\
		\Large \@subtitulo
		%\vskip\baselineskip
				
		%\vskip\baselineskip
		%\large\MakeUppercase{\@tipodetexto}\\
		\vfill % usa todo espaço vertical restante da página
		\@cidade\\
		\@date
	\end{center}
}

\def\capa{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\begin{center}
		\@brasao\\ %\includegraphics[width=2.61cm]{logo.png}
		\sf\normalsize\MakeUppercase{\@instituicao}\\
		\if\@campusmostrar1
			\@campus\\
		\fi
		\textbf{\@setor }
		\vskip 35mm
%		\@curso
		\vskip 25mm
		\begin{minipage}{150mm}
			\begin{center}
				\Large \MakeUppercase{\textbf{\@titulo}}
				\vskip\baselineskip
				\@autor
				\vskip\baselineskip
				\large\MakeUppercase{\@tipodetexto}\\
				%\@intx
			\end{center}
		\end{minipage}
		\vfill
		\@cidade\\
		\@date
	\end{center}
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% FOLHA DE ROSTO
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\def\printFolhaderosto{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\setcounter{page}{1} % a contagem das páginas começará a partir dessa folha de rosto
	\begin{center}
		\sf
		\large
		\textbf{\MakeUppercase{\@instituicao}}\\
		\if\@campusmostrar1
		\@campus\\
		\fi
		\@setor
		\vskip 20mm
		\@autor
		\vskip 20mm
		\vskip\baselineskip
		\textbf{\MakeUppercase{\@titulo}}\\
		\Large \@subtitulo
		\vskip 28mm
		\begin{flushright}
			\begin{minipage}{0.5\textwidth}
				%\quote font %
				\large \@textoapresentacao
			\end{minipage}
		\end{flushright}
		\begin{center}
			\vskip 28mm
			\if\@orientadormostrar1
			{Orientador:} \@orientador
			\fi
			\if\@orientadoramostrar1
			{Orientadora:} \@orientadora
			\fi
			
			\if\@coorientadormostrar1
			{Coorientador:} \@coorientador
			\fi
			\if\@coorientadoramostrar1
			{Coorientadora:} \@coorientadora
			\fi
		\end{center}
		\vfill
		\@cidade\\
		\@date
	\end{center}
}

\def\folhaderostonew{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\setcounter{page}{1} % a contagem das páginas começará a partir dessa folha de rosto
	\begin{center}
		\sf\large
		\textbf{\MakeUppercase{\@instituicao}}\\
		\if\@campusmostrar1
			\@campus\\
		\fi
		\@setor
		\vskip 25mm
		\@autor
		\vskip\baselineskip
		\textbf{\MakeUppercase{\@titulo}}\\
		\Large \@subtitulo
		\vskip 58mm
		\begin{flushright}
			\begin{minipage}{100mm}
%				\quote font %
				\@textoapresentacao
			\end{minipage}
		\end{flushright}
		\begin{center}
			\vskip\baselineskip
			\if\@orientadormostrar1
				{Orientador:} \@orientador
			\fi
			\if\@orientadoramostrar1
				{Orientadora:} \@orientadora
			\fi
			
			\if\@coorientadormostrar1
				{Coorientador:} \@coorientador
			\fi
			\if\@coorientadoramostrar1
				{Coorientadora:} \@coorientadora
			\fi
		\end{center}
		\vfill
		\@cidade\\
		\@date
	\end{center}
}

% esse protótipo não está sendo usado
\def\folhaderostoo{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\begin{center}
		\sf\large
%		\@instituicao\\
%		\if\@campusmostrar1
%		\@campus\\
%		\fi
%		\@setor
%		\vskip 25mm
		\MakeUppercase{\@autor}
		\vskip 50mm
		\vskip\baselineskip
		\textbf{\MakeUppercase{\@titulo}}
		\vskip 50mm
		\begin{flushright}
			\begin{minipage}{90mm}
				\@textoapresentacao
				\begin{flushleft}
					\vskip2\baselineskip
					{\@orientadortitulo:} \@orientadornome
				\end{flushleft}
			\end{minipage}
		\end{flushright}
		\vfill
		\@cidade\\
		\@date
	\end{center}
}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% 		FICHA CATALOGRÁFICA
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\def\biblioteca#1{%
	\gdef\@biblioteca{#1}}
\biblioteca{Biblioteca Universitária do CMBJL}
\def\cutter#1{%
	\gdef\@cutter{#1}}
\def\autornomecitacao#1{% 
	\gdef\@autornomecitacao{#1}}
\def\orientadornomecitacao#1{%
	\gdef\@orientadornomecitacao{#1}}
\def\coorientadornomecitacao#1{%
	\gdef\@coorientadornomecitacao{#1}}
\def\catalogotipo#1{%
	\gdef\@catalogotipo{#1}}
\def\catalogotopicos#1{%
	\gdef\@catalogotopicos{#1}}
\def\cdd#1{%
	\gdef\@cdd{#1}}

\def\fichacatalografica{%
	\begin{center}
		\if@openright\cleardoublepage\else\clearpage\fi
		\thispagestyle{empty}
		\hspace{1.5cm}
		\vfill
		Ficha catalogr\'{a}fica / \@biblioteca.
		\fbox{
			\hspace{1.75cm}
			\begin{minipage}[t]{12.5cm}
				\vspace{1em}
				\vspace{0.1cm} \hspace{-1.75cm} \@cutter \hspace{0.38cm} \@autornomecitacao\\[1em] % 1em =~ 3.51mm 
				\vspace{0.01cm} \hspace{0.1cm}
%				\setlength{\parindent}{1,15cm}
%				\setlength{\parskip}{6pt}
				\advance\parindent by 1em
				\advance\parskip by 1em
%				\setlength{\parskip}{6pt}
				\hspace{0.4cm}\@biblioteca: \@titulo / \@autor\ -- \@cidade, \@date. \\
				\vspace{0.5cm}
				\hspace{0.25cm} 
				\pageref{LastPage}p.: il. color \\ 
				\if\@orientadormostrar1
					\vspace{0.01cm} \hspace{0.5cm} {Orientador:} \@orientador\\
				\fi
				\if\@orientadoramostrar1
					\vspace{0.01cm}\hspace{0.5cm} {Orientadora:} \@orientadora\\
				\fi	
				\if\@coorientadormostrar1
					\vspace{0.01cm} \hspace{0.5cm} {Coorientador:} \@coorientador\\
				\fi
				\if\@coorientadoramostrar1
					\vspace{0.01cm} \hspace{0.5cm} {Coorientadora:} \@coorientadora\\
				\fi
				{\vspace{0.1cm}\hspace{0.41cm} \@catalogotipo\ -- \@instituicao, \if\@campusmostrar1
					\@campus,
					\fi \@setor, \@date.}
				\newline
				\newline
%				\hspace{0.5cm} 
				\@catalogotopicos.
				I. \@orientadornomecitacao.
				\if\@coorientadormostrar1
					II. \@coorientadornomecitacao.\
					III. \@instituicao. \ % \@setor.
					IV. T\'{i}tulo.
				\else
					II. \@instituicao.% \@setor.
					{ }III. T\'{i}tulo.\\
				\fi
				%\newline
				\vspace{0.01cm} %\hspace{9.85cm}
				\hfill CDD: \@cdd %\hfill alinha o texto a direita
			\end{minipage}
		}
	\end{center}
}%

\def\printfichacatalografica{%
	\if\@optrabalho0
		\input{___pretextual/ficha_catalografica}
		\fichacatalografica		
	\fi
	
	\if\@optrabalho2
		\input{___pretextual/ficha_catalografica}
		\fichacatalografica		
	\fi   
}
	

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% FOLHA DE APROVAÇÃO
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\def\addBancaOrientadorNew#1#2{% 
	\vspace{2em}
	\hrule
	\vspace{0.25em}
	{#1} 
	{(#2)\\
	 \if\@orientadormostrar1
	 	{(Orientador)}
	 \fi
	 \if\@orientadoramostrar1
	 	{(Orientadora)}
	 \fi
  	}\\
}%


\def\addBancaCoorientador#1{% 
	\vspace{2em}
	\hrule
	\vspace{0.25em}
	{   
		\if\@coorientadormostrar1
		{\@coorientador{ (#1)}\\ (Coorientador)}
		\fi
		\if\@coorientadoramostrar1
		{\@coorientadora{ (#1)}\\ (Coorientadora)}
		\fi
	}\\
}%

\def\addBancaOrientador{% 
	\vspace{2em}
	\hrule
	\vspace{0.25em}
	{   
		\if\@orientadormostrar1
		{\@orientador{ (UFOB)}\\ (Orientador)}
		\fi
		\if\@orientadoramostrar1
		{\@orientadora{ (UFOB)}\\ (Orientadora)}
		\fi
	}\\
}%

\def\addBancaCoorientadorNew#1#2{% 
	\vspace{2em}
	\hrule
	\vspace{0.25em}
	{#1} 
	{(#2)\\
		\if\@coorientadormostrar1
		{(Coorientador)}
		\fi
		\if\@coorientadoramostrar1
		{(Coorientadora)}
		\fi
	}\\
}%


\def\addBanca#1#2{% 
	\vspace{2em}
	\hrule
	\vspace{0.25em}
	{#1} {(#2)}\\
}%



\def\folhadeaprovacao#1#2{%
%	\newpage
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	\begin{center}
		{\sf\large \textbf{FOLHA DE APROVA\c{C}\~{A}O}}\\
		\vspace{3em}
		{\sf\large\textbf{\MakeUppercase{\@autor}}}\\
		\vspace{3em}
		\begin{minipage}{0.7\textwidth}
			\centering
			\sf\large\textbf{\MakeUppercase{\@titulo}}
		\end{minipage}
		\vspace{3em}
		\begin{flushright}
			\begin{minipage}{0.5\textwidth}
				\@textoaprovacao
			\end{minipage}
		\end{flushright}
		
		\vspace{3em}
		\begin{center}
			{#1}
		\end{center}
		
		\begin{minipage}{0.7\textwidth}
			\centering
			{#2}
		\end{minipage}
		
		\vfill
	\end{center}
}%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% DEDICATÓRIA - forma diferente de criar uma novo ambiente
%				\begin{dedicatoria}
%					conteúdo...
%				\end{dedicatoria}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\def\dedicatoria{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	~\\
	\vfill
	\begin{flushright}
		\sl
		\begin{minipage}{100mm}}
%			\quotefont
			\def\enddedicatoria{
			\normalfont
		\end{minipage}
	\end{flushright}}
	

	
	
\def\iniciodedicatoria{%
	\if@openright\cleardoublepage\else\clearpage\fi
	\thispagestyle{empty}
	~\\* % \\* diferente de \\
	\vfill
	\begin{flushright}
		\sl
		\begin{minipage}{100mm}
}
					
\def\finaldedicatoria{
	\normalfont
		\end{minipage}
	\end{flushright}
}
		
\def\inicioepigrafe{%
\if@openright\cleardoublepage\else\clearpage\fi
\thispagestyle{empty}
~\\
\vfill
\begin{flushright}
	\sl
	\begin{minipage}{90mm}}
	
	\def\finalepigrafe#1{
		\normalfont
		\flushright(#1)
	\end{minipage}
\end{flushright}}

\def\printEpigrafe{
	\if\@optrabalho0
		\include{___pretextual/epigrafe}
	\fi
	
	\if\@optrabalho2
		\include{___pretextual/epigrafe}
	\fi
}		
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% DEDICATÓRIA - forma diferente de criar uma novo ambiente
%				\begin{dedicatoria}
%					conteúdo...
%				\end{dedicatoria}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%	
	\newenvironment{Dedicatoria}
	{% o conteúdo do que vem depois do \begin{comando}
		\if@openright\cleardoublepage\else\clearpage\fi
		\thispagestyle{empty}
		~\\
		\vfill
		\begin{flushright}
			\sl
			\begin{minipage}{100mm}
	} {% o conteúdo do que vem antes do \end{comando}
			\normalfont
			\end{minipage}
		\end{flushright}
	}
	
	\def\agradecimentos{%
		\if@openright\cleardoublepage\else\clearpage\fi
		%\thispagestyle{empty}
		\chapter*{Agradecimentos}
		\thispagestyle{empty}
	}

	\def\printAgradecimentos{
		\if\@optrabalho0
			\agradecimentos
				\input{___pretextual/agradecimentos}
		\fi
		
		\if\@optrabalho2
			\agradecimentos
				\input{___pretextual/agradecimentos}
			\fi
	}
	
	
	\def\resumo{%
		\if@openright\cleardoublepage\else\clearpage\fi
		\chapter*{Resumo}
	    \thispagestyle{empty}
     }	
		
	\def\palavraschaveOld#1{
		\vspace{8mm}\\
		\textbf{Palavras-chave: }#1.
	}

   \def\palavraschave#1{% 
   		\vspace{8mm}
   		\noindent
   		{\textbf{Palavras-chave:} } {#1}
   }%

	
	\def\printResumo{
		\resumo 
		\noindent
		\input{___pretextual/resumo}
		\newline
	}

 	
	
	\def\abstract{%
		\if@openright\cleardoublepage\else\clearpage\fi
		\chapter*{Abstract}
		\thispagestyle{empty}
	}		
	\def\keywords#1{
		\vspace{8mm}
		\noindent
		\textbf{Keywords: }#1.}
			
	\def\printAbstract{
		\if\@optrabalho0
			\abstract 
			\noindent
			\input{___pretextual/abstract}
			\newline
		\fi
		
		\if\@optrabalho2
			\abstract 
			\noindent
			\input{___pretextual/abstract}
			\newline
		\fi
	}			
	
\def\anexo{ %
	\setcounter{chapter}{0}%
	\renewcommand\appendixname{ANEXO}
}	

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% FORMATAÇÃO DE ESTILOS CAPÍTULOS, SEÇÕES E ETC
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\if\@opestilo0
	\titleformat{\chapter}[display]
	{\normalfont\Large\bfseries}
	{}
	{0pt}
	{%
		\begin{tikzpicture}
		\node[draw,thick,rounded corners=5pt,,text width=\the\dimexpr\textwidth-12pt\relax,inner ysep=10pt]
		(chaptitle) 
		{%
			\\\parbox{3.5cm}{\centering\Huge \sf \thechapter}%
			\parbox{\dimexpr\linewidth-3cm\relax}{\sf \MakeUppercase{\textbf{#1}}}%
		};
		\node[anchor=west,fill=white] 
		at ([xshift=15pt]chaptitle.north west) {\chaptertitlename};
		\end{tikzpicture}%
	}
\fi

\if\@opestilo1
	%\renewcommand{\thechapter}{\Roman{chapter}}
	\titleformat{\chapter}[display]
	{\bfseries\Large}
	{\filleft\MakeUppercase{\chaptertitlename} \Huge\thechapter}
	{4ex}
	{\titlerule
		\vspace{2ex}%
		\MakeUppercase{\textbf{#1}}
		\filright}
	[\vspace{2ex}%
	\titlerule]
\fi
	\if\@opestilo2
	\def\thickhrulefill{\leavevmode \leaders \hrule height 1.2ex \hfill \kern \z@}
	\def\@makechapterhead#1{
		\vspace*{10\p@}%
		{\parindent \z@ \centering \reset@font
			\thickhrulefill\quad 
			\scshape\bfseries\textit{\@chapapp{}  \thechapter}  
			\quad \thickhrulefill
			\par\nobreak
			\vspace*{10\p@}%
			\interlinepenalty\@M
			\hrule
			\vspace*{10\p@}%
			\Huge \bfseries #1 \par\nobreak
			\par
			\vspace*{10\p@}%
			\hrule
			\vskip 100\p@
		}}
\fi

\if\@opestilo3
	\definecolor{ufobchapterbgcolor}{rgb}{0.3,0.3,0.3} % Ademário : altera a cor da indicação capítulo 
	\def\@makechapterhead#1#2{%
		\begin{flushleft}
%			\begin{tabular}{|c|c|}
%				\hline 
%					\begin{minipage}{10mm}
%						???
%					\end{minipage} 
%				& 
%				2 \\ 
%				\hline 
%				3 
%				& 
%				4 \\ 
%				\hline 
%			\end{tabular} 
			
			\colorbox{ufobchapterbgcolor}{
				\begin{minipage}{33mm}
					\begin{center}
						\vspace{0.5\baselineskip}
						{\sffamily\bfseries \fontsize{18pt}{18pt}\selectfont \color{white}\textbf{\chaptername}} \\  % Ademário : altera a formação da fonte do "nome capítulo"
						\vspace{5mm}
						{\sffamily\bfseries \fontsize{78pt}{78pt}\selectfont \color{white}{\textbf{\thechapter}}} \\ \vspace{0.5\baselineskip} % Ademário : altera a formação da fonte do número do capítulo
					\end{center}
				\end{minipage}
			}
		\end{flushleft}
		\vskip 0.5\baselineskip%
		{\noindent\itshape\footnotesize #2}
		\vskip 0.5\baselineskip%
		{\begin{flushright}\sffamily\Large\bfseries\MakeUppercase{#1}\end{flushright}}% Ademário : altera a formatação do título do capítulo (flushright lushleft)
		\vskip 2.5\baselineskip
	}
\fi

\if\@opestilo4
	\definecolor{myblue}{RGB}{0,0,30}
	%\definecolor{myblue}{RGB}{120,0,30}
	\titleformat{\chapter}[display]
	{
		\normalfont
		\bfseries
		\color{myblue}
	}
	{
		\filleft%
		\begin{tikzpicture}
			\node[
				outer sep=0pt,
				text width=2.5cm,
				minimum height=3.1cm,
				fill=myblue,
				font=\color{white}\fontsize{80}{90}\selectfont,
				align=center
			] (caixa) {\thechapter}; % \thechapter é o número do capítulo
			\node[
				rotate=90,
				anchor=south,
				font=\color{black}\Large\normalfont
			] at ([xshift=-5pt]caixa.west) {{\textsc{\chaptertitlename}}}; 
			%\chaptertitlename é o "NOME Capítulo" 
		\end{tikzpicture}%
	}
	{10pt}
	{
		\titlerule[2.5pt]
		\vskip3pt 
		\titlerule
		\vskip4pt
		\LARGE
		\sffamily #1
	}
\fi

% \thechapter é o número do capítulo
% \chaptertitlename é o "NOME Capítulo"
\if\@opestilo5
	\definecolor{mycolor}{RGB}{0,0,0}
	\definecolor{cor}{RGB}{160,200,200}
	\titleformat{\chapter}[display]
	{\normalfont\bfseries\color{cor}}
	{\filright%
		\begin{tikzpicture}
%			\tikzstyle{every node}=[draw,shape=circle];
			\node[outer sep=0pt, text width=1.5cm, minimum height=4cm,
			fill=cor, font=\color{white}\sffamily\fontsize{60}{80}\selectfont,
			align=center] (caixanum) {\thechapter};
			\draw[fill=mycolor!100!red, align=right] (-1.85,-0.85) rectangle (-0.9,1.994) (0,0) node[rotate=90, font=\color{cor}\Large\sffamily] at (-1.4,0.5){\textsc{\chaptertitlename}}(6,0) node[align=left] at ([xshift=+150pt]caixanum.east) {\LARGE\sffamily\MakeUppercase{#1}};
			\draw[line width=0.1cm] (0.9,-0.5) -- (14,-0.5);
			\draw[line width=0.05cm] (0.9,-0.6) -- (14,-0.6);
		\end{tikzpicture}
	}
	{0pt}
	{}
%    {\titlerule[2.5pt]\vskip 3pt \titlerule\vskip4pt\LARGE\sffamily #1}
\fi	

\if\@opestilo6
	%	\definecolor{myblue}{RGB}{50,100,120}
	\definecolor{myblue}{RGB}{80,80,80}
	\titleformat{\chapter}[display]
	{\normalfont\bfseries\color{myblue}}
	{\filright%
		\begin{tikzpicture}
		\node[
		outer sep=0pt,
		text width=2.5cm,
		minimum height=3cm,
		fill=myblue,
		font=\color{white}\fontsize{80}{90}\selectfont,
		align=right
		] (caixa) {\thechapter}; % \thechapter é o número do capítulo
		\node[
		rotate=90,
		anchor=south,
		font=\color{black}\Large\normalfont
		] at ([xshift=20pt]caixa.west) {\textcolor{white}{\textsc{\chaptertitlename}}}; 
		%\chaptertitlename é o "NOME Capítulo" 
		\end{tikzpicture}%
	}
	{10pt}
	{\titlerule[2pt]\vskip3pt \titlerule\vskip4pt\LARGE\sffamily \flushright \textcolor{black}{#1}\vskip4pt \titlerule[2pt] \vskip13pt}
\fi