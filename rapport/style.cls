
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{style}[14/09/2010 v2.0]

%\LoadClass[twoside,12pt,a4paper]{report}	% notez le "twoside", essentiellement pour alterner les en-tete et pieds de page gauche et droit si on veut utliser les fancyhead.
\LoadClass[12pt,a4paper]{article}

\RequirePackage[utf8]{inputenc}
\RequirePackage[french]{babel}
\RequirePackage{ifpdf, color}
\RequirePackage{textcomp}
\RequirePackage[document]{ragged2e}
\RequirePackage{enumerate}

\RequirePackage[T1]{fontenc}

\RequirePackage{lmodern} % Assurer une bonne impression!
\RequirePackage{tikz} % tikz est utilise pour tracer des boites, par exemple
\RequirePackage{fix-cm}
\RequirePackage{graphicx} % Pour insérer des images. Utiliser le format jpg pour plus de simplicité.
\RequirePackage{sectsty} % obligatoire, pour jouer avec l'apparence des titres
\RequirePackage{shorttoc} % pour le sommaire, au début.
\RequirePackage{fancyhdr} % Afin de réaliser soi-même les en-têtes et pieds de page, voir chaque début de chapitre.
\RequirePackage{pifont} % Pour utiliser des symboles divers.

%\RequirePackage[inner=30mm,outer=20mm,top=30mm,bottom=20mm]{geometry} % à utiliser avec le LoadClass "twoside"
\RequirePackage[inner=20mm,outer=20mm,top=20mm,bottom=20mm]{geometry}

%Ou bien : \RequirePackage[top=4 cm, bottom=4 cm, left=3 cm, right=3 cm]{geometry} % On peut modifier ces valeurs pour augmenter ou réduire les marges. Attention aux en-têtes de chapitres. 

\RequirePackage{setspace} % permet d'utiliser les commandes \spacing, doublespace (double interligne), singlespace (simple interligne) et onehalfspace (un interligne et demi)

\RequirePackage{hyperref} %Utiliser pour une bibliographie interactive + sommaire et TOC interactifs (pour une sortie PDF, par exemple).
\hypersetup{colorlinks=true, citecolor=black, filecolor=black, linkcolor=black, urlcolor=black}

% A ajouter dans hypersetup pour plus de visibilite dans les proprietes du document
%pdftitle={titre du document}, pdfauthor={auteur}, pdfcreator={PdfLaTeX}, pdfkeywords={mots-clé}, pdfsubject={sujet du document}

\RequirePackage{framed} % utilise pour coloriser les encarts
\RequirePackage{color} % pour les couleurs

\onehalfspacing %Interligne 1,5

\RequirePackage{ifthen} % Entrer valeurs booléennes et autres options

\RequirePackage{csquotes} % Assurer les guillemets français

\RequirePackage{listings} % Pour les algos
\RequirePackage{amsmath}
\RequirePackage{stmaryrd}
\frenchspacing
\FrenchFootnotes
\usetikzlibrary{arrows,automata}
\RequirePackage{algorithmic}


%%%%%%%%%%%%%% En-tete de chapitre %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Les lignes suivantes définissent l'esthétique des débuts de chapitre.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\makeatletter
\def\thickhrulefill{\leavevmode \leaders \hrule height 1ex \hfill \kern \z@}
\def\@makechapterhead#1{%
  \vspace*{-30\p@}%
  {\parindent \z@ \raggedleft \reset@font
            \scshape %\@chapapp{} \thechapter
        \par\nobreak
        \interlinepenalty\@M
    \Huge \bfseries #1\par\nobreak
    %\vspace*{1\p@}%
    \hrulefill
    \par\nobreak
    \vskip 10\p@
  }}
\def\@makeschapterhead#1{%
 \vspace*{-50\p@}%
  {\parindent \z@ \raggedleft \reset@font
            \scshape \vphantom{\@chapapp{} \thechapter}
        \par\nobreak
        \interlinepenalty\@M
    \Huge \bfseries #1 \par\nobreak
    %\vspace*{1\p@}%
    \hrulefill
    \par\nobreak
    \vskip 30\p@
  }}

%%%%%%%%%%%%%%%%%%%% Fin en-tête de chapitre %%%%%%%%%%%%%%%%%%%

%%%%%%%%%%% Tete de parties %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\makeatletter
\newcommand*{\jolipart}[1]{%
	\begin{center}
		\begin{Huge}\color{black}
			#1
		\end{Huge}
		\end{center}
		\vspace{1cm}		
	\begin{center}
%\begin{Huge}
		%\ding{167}
	%\end{Huge}	
	\hrulefill
	\end{center}		
	}


\parttitlefont{\jolipart}

\partnumberfont{\sc\normalsize}



%%%%%%%%%%%%%%%% COULEURS grises %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\definecolor{grisfonce}{RGB}{49,49,49}
\definecolor{grisclair}{RGB}{111,111,111}
\definecolor{blanc}{RGB}{255,255,255}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




%%%%%%%%%%%%%%%% TITRE THESE PAGE DE GARDE %%%%%%%%%%%%%%%%

%logo universite

\newcommand*{\logouniversite}[1]{\gdef\@logouniversite{#1}}
\logouniversite{}

\newcommand*{\scalelogouniversite}[1]{\gdef\@scalelogouniversite{#1}}
\scalelogouniversite{}

% Logo projet
\newcommand*{\logoprojet}[1]{\gdef\@logoprojet{#1}}
\logoprojet{}

\newcommand*{\scalelogoprojet}[1]{\gdef\@scalelogoprojet{#1}}
\scalelogoprojet{}

% Nom de l'universite
\newcommand*{\nomdeuniversite}[1]{\gdef\@nomdeuniversite{#1}}
\nomdeuniversite{}

%%%% Eventuel sous-titre
\newcommand{\lesoustitre}[1]{\gdef\@lesoustitre{#1}}
\lesoustitre{}

%%%% Discipline
\newcommand{\discipline}[1]{\gdef\@discipline{#1}}
\discipline{}

%%%% Nom du directeur de these
\newcommand{\encadrant}[1]{\gdef\@encadrant{#1}}
\encadrant{}

%%%% Titre du directeur de these
\newcommand{\titreencadrant}[1]{\gdef\@titreencadrant{#1}}
\titreencadrant{}

%%%% Nom du directeur de these
\newcommand{\client}[1]{\gdef\@client{#1}}
\client{}

%%%% Titre du directeur de these
\newcommand{\titreclient}[1]{\gdef\@titreclient{#1}}
\titreclient{}


%%%% Jury
\newcommand{\jury}[1]{\gdef\@jury{#1}}
\jury{}

%%%% Laboratoire / department / unité
\newcommand{\unite}[1]{\gdef\@unite{#1}}
\unite{}

%%%%% Ecole doctorale
\newcommand{\ecoledoc}[1]{\gdef\@ecoledoc{#1}}
\ecoledoc{}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%mise en page de la page de garde
\makeatletter
\def\maketitle{%
	\thispagestyle{empty}
	\clearpage
	\begin{tikzpicture}[remember picture,overlay,line width=0mm]
  \draw [draw=white,fill=white]
    (current page.north west) rectangle (\paperwidth,1);
  \node[xshift=.15\paperwidth,yshift=-0.5cm,text=white,font=\bf\Large] {
  \includegraphics[scale=\@scalelogouniversite]{\@logouniversite}
  };
  \node[xshift=.68\paperwidth,yshift=-0.5cm,text=white,font=\bf\Large] {
  \includegraphics[scale=\@scalelogoprojet]{\@logoprojet}
  };
   \node[yshift=-2cm] at (current page.center)
{\parbox{15cm}{
\begin{center}
\begin{huge}
\ding{117} \ding{117} \ding{117} % To add symbols with Latex
\end{huge}
\end{center}
\begin{center}

\@unite \\
\@ecoledoc
\end{center}
% 	\begin{center}
% 		\textbf{Discipline : \@discipline}
% 	\end{center}
\vspace{1.5cm}
\noindent\rule{15cm}{0.5mm}
	\begin{center}
		{\setlength{\baselineskip}{1.4\baselineskip}
		\begin{huge}
			\textbf{\@title}
		\end{huge}
		\par}
	\end{center}
	\begin{center}
		
		\textbf{\Large\@lesoustitre}
	\end{center}
\noindent\rule{15cm}{0.5mm}
	\begin{flushright}
		\textsc{Par : \textbf{\@author}}
	\end{flushright}
\vspace{0.5cm}
\bigskip
	\begin{flushleft}
	    \textbf{Client  :} \textsc{\@client} \@titreclient \\
		\textbf{Encadrant  :} \textsc{\@encadrant} \@titreencadrant
	\end{flushleft}
}
};
\end{tikzpicture}
}

\makeatother



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%% En-tetes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%% Pour supprimer les entetes et pied de page gênants par exemple juste avant un chapitre sur une page de droite
\newcommand{\clearemptydoublepage}{%
   \newpage{\pagestyle{empty}\cleardoublepage}}
%%%% .... et utiliser la commande \clearemptydoublepage juste avant le \chapter


    \fancyhf{}                       % on annule le fancy automatique
    

   

%%%%%%%%%% Gerer les en tetes dans les frontmatter mainmatter et backmatter

\RequirePackage{etoolbox}

\appto\frontmatter{\pagestyle{fancy}
\renewcommand{\sectionmark}[1]{}
\renewcommand{\chaptermark}[1]{\markboth{\textit{#1}}{}}
\fancyhead[LE,RO]{\small\thepage}
    \fancyhead[RO]{\small\leftmark}   % \rightmark = section courante
    \fancyhead[LE]{\small\leftmark} % \leftmark = chapitre courant
    \fancyfoot[C]{\thepage}               % marque la page au centre
}

\appto\mainmatter{\pagestyle{fancy}
\renewcommand{\sectionmark}[1]{\markright{\textit{\thesection.\ #1}}}
\renewcommand{\chaptermark}[1]{\markboth{\chaptername~\thechapter~--\ \textit{#1}}{}}
%\fancyhead[LE,RO]{\small\thepage}
    \fancyhead[RO]{\small\rightmark}   % \rightmark = section courante
    \fancyhead[LE]{\small Partie~\thepart, \leftmark} % \leftmark = chapitre courant
    \fancyfoot[C]{\thepage}               % marque la page au centre
}     

\appto\backmatter{\pagestyle{fancy}
\renewcommand{\sectionmark}[1]{\markright{\thesection.\ #1}}
%\renewcommand{\chaptermark}[1]{\markboth{\chaptername~\thechapter~--\ #1}{}}
\fancyhead[LE,RO]{\small\thepage}
    \fancyhead[RO]{}   % \rightmark = section courante
    \fancyhead[LE]{} % \leftmark = chapitre courant
    \fancyfoot[C]{\thepage}               % marque la page au centre

} 


%%%%%%%%%%%%%% FAIRE DES ENCARTS %%%%%%%%%%%%%%%%%%%%%%%%%%
% Couleurs :


\definecolor{fondtitre}{RGB}{85,85,85}
\definecolor{fonddeboite}{RGB}{232,232,232}
\definecolor{shadecolor}{RGB}{232,232,232}

\ProcessOptions


\renewcommand{\algorithmicrequire}{\textbf{Require:}}
\renewcommand{\algorithmicensure}{\textbf{Ensure:}}
\renewcommand{\algorithmicend}{\textbf{fin}}
\renewcommand{\algorithmicif}{\textbf{Si}}
\renewcommand{\algorithmicthen}{\textbf{alors}}
\renewcommand{\algorithmicelse}{\textbf{Sinon}}
\renewcommand{\algorithmicelsif}{\algorithmicelse\ \algorithmicif}
\renewcommand{\algorithmicendif}{\algorithmicend\ \algorithmicif}
\renewcommand{\algorithmicfor}{\textbf{Pour}}
\renewcommand{\algorithmicforall}{\textbf{Pour tout}}
\renewcommand{\algorithmicdo}{\textbf{faire}}
\renewcommand{\algorithmicendfor}{\algorithmicend\ \algorithmicfor}
\renewcommand{\algorithmicwhile}{\textbf{Tant que}}
\renewcommand{\algorithmicendwhile}{\algorithmicend\ \algorithmicwhile}
\renewcommand{\algorithmicloop}{\textbf{loop}}
\renewcommand{\algorithmicendloop}{\algorithmicend\ \algorithmicloop}
\renewcommand{\algorithmicrepeat}{\textbf{repeat}}
\renewcommand{\algorithmicuntil}{\textbf{until}}
\renewcommand{\algorithmicprint}{\textbf{print}}
\renewcommand{\algorithmicreturn}{\textbf{return}}
\renewcommand{\algorithmictrue}{\textbf{vrai}}
\renewcommand{\algorithmicfalse}{\textbf{false}}