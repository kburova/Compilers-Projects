%{
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
    printf( "\\begin{verbatim}\n");
    BEGIN PRE;
}
<PRE>[.*] {
    printf( "%s", yytext );
}

<PRE>"</pre>" {
    printf( "\n\\end{verbatim}\n");
    BEGIN 0;
}

"<ol>" {
    printf( "\\begin{enumerate}");
    list_type = 0;
    BEGIN OL;
}
"<ul>" {
    printf( "\\begin{itemize}");
    list_type = 1;
    BEGIN UL;
}
<OL>"<li>" {
    printf( "\\item ", yytext);
    BEGIN LI;
}
<UL>"<li>" {
    printf( "\\item ", yytext);
    BEGIN LI;
}
<LI>[^<]* {
    printf( "%s", yytext );
}
<LI>"</li>" {
    if (list_type == 1)
        BEGIN UL;
    else
        BEGIN OL;
}
<OL>"</ol>" {
    printf( "\\end{enumerate}\n");
    BEGIN 0;
}
<UL>"</ul>" {
    printf( "\\end{itemize}\n");
    BEGIN 0;
}

"<p>" {
    BEGIN PARA;
}
<PARA>[^<]* {
    printf( "%s", yytext );
}
<PARA>"<big>" {
    printf( "{\\Large ", yytext );
}
<LI>"<big>" {
    printf( "{\\Large ", yytext );
}
<PARA>"<small>" {
    printf( "{\\scriptsize ", yytext );
}
<LI>"<small>" {
    printf( "{\\scriptsize ", yytext );
}
<PARA>"<b>" {
    printf( "\\textbf{", yytext );
}
<LI>"<b>" {
    printf( "\\textbf{", yytext );
}
<PARA>"<i>" {
    printf( "\\textit{", yytext );
}
<LI>"<i>" {
    printf( "\\textit{", yytext );
}
<PARA>"<strong>" {
    printf( "\\textmd{", yytext );
}
<LI>"<strong>" {
    printf( "\\textmd{", yytext );
}
<PARA>"<em>" {
    printf( "\\emph{", yytext );
}
<LI>"<em>" {
    printf( "\\emph{", yytext );
}
<PARA>"<sup>" {
    printf( "\\textsuperscript{", yytext );
}
<LI>"<sup>" {
    printf( "\\textsuperscript{", yytext );
}
<PARA>"<sub>" {
    printf( "\\textsubscript{", yytext );
}
<LI>"<sub>" {
    printf( "\\textsubscript{", yytext );
}

<PARA>"</"(big|small|b|i|strong|em|sup|sub)">" {
    printf( "}", yytext );
}

<LI>"</"(big|small|b|i|strong|em|sup|sub)">" {
    printf( "}", yytext );
}

<PARA>"</p>" {
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
    printf( "\\usepackage{itemized}\n" );
    printf( "\n" );
    printf( "\\setlength{\\parindent}{0.0in}\n" );
    printf( "\\setlength{\\parskip}{0.125in}\n" );
    printf( "\n\n" );
    printf( "\\begin{document}\n" );
    yylex();
    printf( "\\end{document}\n" );
    return 0;
}
