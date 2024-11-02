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

[^ \t\n><|&][^ \t\n><|&]*  {
                    yylval.string_val = strdup(yytext);
                    return WORD;
}

[><|&]         { return NOTOKEN; }  /* Catch single operators as errors */

%%