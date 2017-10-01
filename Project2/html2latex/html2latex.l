%{

/*****************************

CS 461. PA2. Lexical Analysis.
Completed by: Ksenia Burova

******************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int list_type;
%}

%option       noyywrap nounput yylineno
%x            COMMENT PARA PRE OL UL LI

%%

"<h1>"[^<]*"</h1>" {
    /**************************************************************************
     * Matched an HTML H1 header so print it out as a LateX section title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\section{%s}\n", yytext+4 );
}
"<h2>"[^<]*"</h2>" {
    /**************************************************************************
     * Matched an HTML H2 header so print it out as a LateX section title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\subsection{%s}\n", yytext+4 );
}
"<h3>"[^<]*"</h3>" {
    /**************************************************************************
     * Matched an HTML H3 header so print it out as a LateX section title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\subsubsection{%s}\n", yytext+4 );
}

"<pre>" {
    /**************************************************************
     * Matched preformatted paragraph. Start verbatim tag in Latex.
     * ************************************************************/ 
    printf( "\\begin{verbatim}\n");
    BEGIN PRE;
}
<PRE>. {
    /**************************************************************
     * Read all chars as they are while in preformatted paragraph
     * and print them.
     * ************************************************************/ 
    printf( "%s", yytext );
}

<PRE>"</pre>" {
    /**************************************************************
     * Match closing for preformatted paragraph. Print closing tag
     * in Latex.
     * ************************************************************/ 
    printf( "\n\\end{verbatim}\n");
    BEGIN 0;
}

"<ol>" {
    /**************************************************************
     * Matched ordered list. Start enumerate tag in Latex. Set flag
     * to 0 to identify where to return after <li>
     * ************************************************************/ 
    printf( "\\begin{enumerate}");
    list_type = 0;
    BEGIN OL;
}
"<ul>" {
    /**************************************************************
     * Matched unordered list. Start itemize tag in Latex. Set flag
     * to 1 to identify where to return after <li>
     * ************************************************************/
    printf( "\\begin{itemize}");
    list_type = 1;
    BEGIN UL;
}
<OL>"<li>" {
    /**************************************************************
     * Matched item in ordered list. Start item tag in Latex. 
     * ************************************************************/
    printf( "\\item ");
    BEGIN LI;
}
<UL>"<li>" {
    /**************************************************************
     * Matched item in unordered list. Start item tag in Latex. 
     * ************************************************************/
    printf( "\\item ");
    BEGIN LI;
}
<LI>"</li>" {
    /**************************************************************
     * Matched closing item tag. Return from item to unordered list
     * or ordered one depending on flag, to close the tag.
     * ************************************************************/
    if (list_type == 1)
        BEGIN UL;
    else
        BEGIN OL;
}
<OL>"</ol>" {
    /**************************************************************
     * Matched ordered list closing tag. Close enumerate in Latex.
     * ************************************************************/
    printf( "\\end{enumerate}\n");
    BEGIN 0;
}
<UL>"</ul>" {
    /**************************************************************
     * Matched unordered list closing tag. Close itemize in Latex.
     * ************************************************************/
    printf( "\\end{itemize}\n");
    BEGIN 0;
}

"<p>" {
    /**************************************************************
     * Matched paragraph tag. Start paragraph.
     * ************************************************************/
    BEGIN PARA;
}

<PARA,LI>[^<]* {
    /**************************************************************
     * Matched any chars but '<' in paragraph or list item. Print 
     * to Latex doc
     * ************************************************************/
    printf( "%s", yytext );
}

<PARA,LI>"<big>" {
    /**************************************************************
     * Matched large font tag in paragraph or list item. Start 
     * Large tag in Latex
     * ************************************************************/
    printf( "{\\Large " );
}

<PARA,LI>"<small>" {
    /**************************************************************
     * Matched small font tag in paragraph or list item. Start 
     * scriptsize tag in Latex
     * ************************************************************/
    printf( "{\\scriptsize " );
}

<PARA,LI>"<b>" {
    /**************************************************************
     * Matched bold font tag in paragraph or list item. Start 
     * bold tag in Latex
     * ************************************************************/
    printf( "\\textbf{" );
}

<PARA,LI>"<i>" {
    /**************************************************************
     * Matched italic font tag in paragraph or list item. Start 
     * italic tag in Latex
     * ************************************************************/
    printf( "\\textit{" );
}

<PARA,LI>"<strong>" {
    /**************************************************************
     * Matched strong font tag in paragraph or list item. Start 
     * md tag in Latex
     * ************************************************************/
    printf( "\\textmd{" );
}

<PARA,LI>"<em>" {
    /**************************************************************
     * Matched emphasis font tag in paragraph or list item. Start 
     * emphasis tag in Latex
     * ************************************************************/
    printf( "\\emph{" );
}

<PARA,LI>"<sup>" {
    /**************************************************************
     * Matched superscript tag in paragraph or list item. Start 
     * superscript tag in Latex
     * ************************************************************/
    printf( "\\textsuperscript{" );
}

<PARA,LI>"<sub>" {
    /**************************************************************
     * Matched subscript tag in paragraph or list item. Start 
     * subscript tag in Latex
     * ************************************************************/
    printf( "\\textsubscript{" );
}

<PARA,LI>"</"(big|small|b|i|strong|em|sup|sub)">" {
    /**************************************************************
     * Matched closing tag for all the above for list and paragraph.
     * Close tags in Latex.
     * ************************************************************/
    printf( "}" );
}

<PARA>"</p>" {
    /**************************************************************
     * Matched closing paragraph tag. Indicate paragraph in Latex
     * ************************************************************/
    printf( " \\par\n");
    BEGIN 0;
}

"<!--" {
    /**************************************************************************
     * Matched an HTML comment so print it out as a LateX comment. The
     * beginning and ending tags are stripped off before the LateX comment
     * is emitted.
     *************************************************************************/
    printf( "%% " );
    BEGIN COMMENT;
}

[ \t\n\r]+ {
    /**************************************************************************
     * Matched some whitespace that is not inside of an HTML tag. We
     * just ignore the whitespace.
     *************************************************************************/
}

. {
    /**************************************************************************
     * Matched something that we did not expect to see. Issue an error and
     * exit the program with a failure code.
     *************************************************************************/
    fprintf( stderr, "Unexpected input at line %d: '%s'\n", yylineno, yytext );
    exit( 1 );
}

<COMMENT>"-->" {
    /**************************************************************************
     * Matched an HTML end comment while parsing an HTML comment. Print out
     * a new line and resume normal parsing.
     *************************************************************************/
    printf( "\n" );
    BEGIN 0;
}

<COMMENT>[\n](\r)? {
    /**************************************************************************
     * Matched a newline while parsing an HTML comment. Print out a new
     * line making sure to mark the next line as a comment as well.
     *************************************************************************/
    printf( "\n%% " );
}

<COMMENT>[ \t] {
    /**************************************************************************
     * Matched some white space while parsing an HTML comment. Print out
     * the white space as is.
     *************************************************************************/
    printf( "%s", yytext );
}

<COMMENT>. {
    /**************************************************************************
     * Matched any other text while parsing an HTML comment. Print out
     * the text as is.
     *************************************************************************/
    printf( "%s", yytext );
}

%%

/**************************************************************************
 * The main function for the HTML to LateX converter. Here we do three
 * things:
 *  1. Print out the LateX document header.
 *  2. Invoke the lexer to print out the LateX document
 *  3. Print out the LateX footer.
 *************************************************************************/
int main( int argc, const char *argv[] ) {
    printf( "\\documentclass[12pt]{article}\n" );
    printf( "\\usepackage{fixltx2e}\n" );
    printf( "\n" );
    printf( "\\setlength{\\parindent}{0.0in}\n" );
    printf( "\\setlength{\\parskip}{0.125in}\n" );
    printf( "\n\n" );
    printf( "\\begin{document}\n" );
    yylex();
    printf( "\\end{document}\n" );
    return 0;
}
