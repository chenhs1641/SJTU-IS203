 /*
  *  The scanner definition for seal.
  */

 /*
  *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
  *  output, so headers and global definitions are placed here to be visible
  * to the code in the file.  Don't remove anything that was here initially
  */
%{

#include <cstring>
#include <seal-parse.h>
#include <stringtab.h>
#include <utilities.h>
#include <stdint.h>
#include <stdlib.h>
using namespace std;
/* The compiler assumes these identifiers. */
#define yylval seal_yylval
#define yylex  seal_yylex

/* Max size of string constants */
#define MAX_STR_CONST 256
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the seal compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE seal_yylval;

/*
 *  Add Your own definitions here
 */
char c;
bool flag = 0, flag0 = 0, flag1 = 0;
char* s;

%}

%option noyywrap
 /*
  * Define names for regular expressions here.
  */

if "if"
else "else"
while "while"
for "for"
break "break"
continue "continue"
func "func"
return "return"
var "var"
and "&&"
or "||"
equal "=="
ne "!="
ge ">="
le "<="
annotation_one "//"[^\n]*
type "Int"|"Float"|"String"|"Bool"|"Void"
INT_CONST 0x([0-9a-fA-F]+)|([0-9]+)
FLOAT_CONST 0x([0-9a-fA-F]+)"."([0-9a-fA-F]+)|([0-9]+)"."([0-9]+)
BOOL_CONST true|false
OBJECT_CONST [a-z]([A-Za-z0-9_]*)
symbol "+"|"-"|"*"|"/"|"%"|"&"|"|"|"^"|"~"|"="|"<"|">"|"!"|";"|"("|")"|"["|"]"|"{"|"}"|","

%%

"`"[^`]*"`" {
  s = new char[512];
  strncpy(s, yytext+1, strlen(yytext)-2);
  for (int i=0;i<strlen(s);i++)
  {
    if (s[i] == '\n') curr_lineno ++;
  }
  if (strlen(s) <= 256)
  {
    seal_yylval.symbol = stringtable.add_string(s);
    delete []s;
    return (CONST_STRING);
  }
  else
  {
    delete []s;
    strcpy(seal_yylval.error_msg, "String too long");
    return ERROR;
  }
}

"\\0" {
  if (flag0)
  {
    flag1 = 1;
    strcpy(seal_yylval.error_msg, "String contains null character '\\0'");
    return (ERROR);
  }
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\n" {
  if (flag0) s = strcat(s, "\n");
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\n" {
  curr_lineno ++;
  if (flag0) s = strcat(s, "\n");
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\n" {
  if (flag0)
  {
    flag1 = 1;
    strcpy(seal_yylval.error_msg, "unescaped newline");
    return (ERROR);
  }
  curr_lineno ++;
}

{annotation_one} {}

"/*" {
  if (flag0) s = strcat(s, yytext);
  else flag = 1;
}

"*/" {
  if (flag0) s = strcat(s, yytext);
  else
  {
    if (flag) 
      flag = 0;
    else
    {
      strcpy(seal_yylval.error_msg, "*/ not matched"); 
	    return (ERROR);
    }
  }
}

"\\t" {
  if (flag0) s = strcat(s, "\t");
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\"" {
  if (flag0) s = strcat(s, "\"");
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\\" {
  if (flag0) s = strcat(s, "\\");
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\"" {
  if (!flag)
  {
    if (!flag0)
    {
      flag0 = 1;
      s = new char[512];
    }
    else if (flag0)
    {
      flag0 = 0;
      if (!flag1)
      {
        if (strlen(s) <= 256)
        {
          seal_yylval.symbol = stringtable.add_string(s);
          delete []s;
          return (CONST_STRING);
        }
        else
        {
          delete []s;
          strcpy(seal_yylval.error_msg, "String too long");
          return (ERROR);
        }
      }
      else
      {
        delete []s;
        flag1 = 0;
      }
    }
  }
}

" " {
  if (flag0) s = strcat(s, yytext);
}

"\t" {
  if (flag0) s = strcat(s, yytext);
}

{symbol}  {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) printf("#%d '%s'\n", curr_lineno, yytext);
}

{if} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (IF);
}

{else} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (ELSE);
}

{while} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (WHILE);
}

{for} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (FOR);
}

{break} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (BREAK);
}

{continue} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (CONTINUE);
}

{func} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (FUNC);
}

{return} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (RETURN);
}

{var} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (VAR);
}

{and} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (AND);
}

{or} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (OR);
}

{equal} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (EQUAL);
}

{ne} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (NE);
}

{ge} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (GE);
}

{le} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag) return (LE);
}

{BOOL_CONST} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
    seal_yylval.boolean = (strcmp(yytext, "true"))?0:1;
    return (CONST_BOOL);
  }
}

{FLOAT_CONST} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
    seal_yylval.symbol = floattable.add_string(yytext);
	  return (CONST_FLOAT);
  }
}

{INT_CONST} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
    seal_yylval.symbol = inttable.add_string(yytext);
	  return (CONST_INT);
  }
}

{OBJECT_CONST} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
    seal_yylval.symbol = idtable.add_string(yytext);
	  return (OBJECTID);
  }
}

{type} {
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
    seal_yylval.symbol = idtable.add_string(yytext);
    return (TYPEID);
  } 
}

.	{
  if (flag0) s = strcat(s, yytext);
  else if (!flag)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

%%