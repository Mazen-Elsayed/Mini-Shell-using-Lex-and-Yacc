/*
 * shell.l: lexical analyzer for shell
 * Extended to support pipes, input/output redirection, and background execution.
 */

%{
#include <string.h>
#include "y.tab.h"
%}

%%

\n              { return NEWLINE; }

[ \t]+         { /* Discard spaces and tabs */ }

">>"           { return APPEND; }
">"            { return OVERWRITE; }
"<"            { return LESS; }
"|"            { return PIPE; }
"&"            { return AMPERSAND; }
">>&"          { return APPEND_ERR; }

[^ \t\n><|&]+  {
    yylval.string_val = strdup(yytext);
    return WORD;
}

%%