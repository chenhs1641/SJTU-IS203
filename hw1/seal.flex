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
bool flag = 0, flag0 = 0, flag1 = 0, flag2 = 0, flag3 = 0;
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
type "Int"|"Float"|"String"|"Bool"|"Void"
INT_CONST 0x([0-9a-fA-F]+)|([0-9]+)
FLOAT_CONST ([0-9]+)"."([0-9]+)
BOOL_CONST true|false
OBJECT_CONST [a-z]([A-Za-z0-9_]*)
false_OBJECT_CONST [A-Z]([A-Za-z0-9_]*)
false_ID [0-9]([A-Za-z0-9_]*)
symbol "+"|"-"|"*"|"/"|"%"|"&"|"|"|"^"|"~"|"="|"<"|">"|"!"|";"|"("|")"|"{"|"}"|","|":"

%%

"`" {
  if (!flag && !flag3)
  {
    if (flag0)
    {
      s = strcat(s, yytext);
      if (strlen(s) > 256)
      {
        flag0 = 0;
        flag2 = 0;
        delete []s;
        strcpy(seal_yylval.error_msg, "String constant too long");
        return (ERROR);
      }
    }
    else if (!flag2)
    {
      flag2 = 1;
      s = new char[512];
    }
    else if (flag2)
    {
      flag2 = 0;
      if (strlen(s) <= 256)
      {
        seal_yylval.symbol = stringtable.add_string(s);
        delete []s;
        return (CONST_STRING);
      }
      else
      {
        delete []s;
        strcpy(seal_yylval.error_msg, "String constant too long");
        return (ERROR);
      }
    }
  }
}

"\\0" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    flag1 = 1;
    strcpy(seal_yylval.error_msg, "String contains null character '\\0'");
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\n" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\n");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\n" {
  curr_lineno ++;
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\n");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, "\\"); 
	  return (ERROR);
  }
  flag3 = 0;
}

"\n" {
  curr_lineno ++;
  flag3 = 0;
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    flag0 = 0;
    flag2 = 0;
    strcpy(seal_yylval.error_msg, "newline in quotation must use a '\\'");
    return (ERROR);
  }
}

"//" {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag) flag3 = 1;
}

"/*" {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag3) flag = 1;
}

"*/" {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag3)
  {
    if (flag) 
      flag = 0;
    else
    {
      strcpy(seal_yylval.error_msg, "Unmatched */"); 
	    return (ERROR);
    }
  }
}

"\\t" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\t");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\b" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\b");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\f" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\f");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\'" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\'");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\?" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\?");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\"" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\"");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\\\\" {
  if (flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (flag0)
  {
    s = strcat(s, "\\");
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

"\"" {
  if (!flag && !flag3)
  {
    if (flag2)
    {
      s = strcat(s, yytext);
      if (strlen(s) > 256)
      {
        flag0 = 0;
        flag2 = 0;
        delete []s;
        strcpy(seal_yylval.error_msg, "String constant too long");
        return (ERROR);
      }
    }
    else if (!flag0)
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
          strcpy(seal_yylval.error_msg, "String constant too long");
          return (ERROR);
        }
      }
      else
      {
        delete []s;
        flag1 = 0;
        return (ERROR);
      }
    }
  }
}

" " {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
}

"\t" {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
}

"\\" {
  if (flag2)
  {
    strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag0 && !flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}

{symbol}  {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (int(yytext[0]));
}

{if} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (IF);
}

{else} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (ELSE);
}

{while} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (WHILE);
}

{for} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (FOR);
}

{break} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (BREAK);
}

{continue} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (CONTINUE);
}

{func} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (FUNC);
}

{return} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (RETURN);
}

{var} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (VAR);
}

{and} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (AND);
}

{or} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (OR);
}

{equal} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (EQUAL);
}

{ne} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (NE);
}

{ge} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (GE);
}

{le} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3) return (LE);
}

{BOOL_CONST} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    seal_yylval.boolean = (strcmp(yytext, "true")) ? 0 : 1;
    return (CONST_BOOL);
  }
}

{FLOAT_CONST} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    seal_yylval.symbol = floattable.add_string(yytext);
	  return (CONST_FLOAT);
  }
}

{INT_CONST} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    char *s0;
    s0 = new char[512];
    strcpy(s0, yytext);
    if (strlen(s0) > 2 && s0[0] == '0')
    {
      if (s0[1] == 'X' || s0[1] == 'x')
      {
        int value = 0, mul = 1, tmp;
        for (int i = strlen(s0) - 1; i >= 2; i --)
        {
          if (s0[i] == '0') value += 0 * mul;
          else if (s0[i] == '1') value += 1 * mul;
          else if (s0[i] == '2') value += 2 * mul;
          else if (s0[i] == '3') value += 3 * mul;
          else if (s0[i] == '4') value += 4 * mul;
          else if (s0[i] == '5') value += 5 * mul;
          else if (s0[i] == '6') value += 6 * mul;
          else if (s0[i] == '7') value += 7 * mul;
          else if (s0[i] == '8') value += 8 * mul;
          else if (s0[i] == '9') value += 9 * mul;
          else if (s0[i] == 'a' || s0[i] == 'A') value += 10 * mul;
          else if (s0[i] == 'b' || s0[i] == 'B') value += 11 * mul;
          else if (s0[i] == 'c' || s0[i] == 'C') value += 12 * mul;
          else if (s0[i] == 'd' || s0[i] == 'D') value += 13 * mul;
          else if (s0[i] == 'e' || s0[i] == 'E') value += 14 * mul;
          else if (s0[i] == 'f' || s0[i] == 'F') value += 15 * mul;
          mul *= 16;
        }
        string S = "";
        while (value > 0)
        {
          tmp = value % 10;
          value /= 10;
          if (tmp == 0) S = "0" + S;
          else if (tmp == 1) S = "1" + S;
          else if (tmp == 2) S = "2" + S;
          else if (tmp == 3) S = "3" + S;
          else if (tmp == 4) S = "4" + S;
          else if (tmp == 5) S = "5" + S;
          else if (tmp == 6) S = "6" + S;
          else if (tmp == 7) S = "7" + S;
          else if (tmp == 8) S = "8" + S;
          else if (tmp == 9) S = "9" + S;
        }
        strcpy(s0, S.c_str());
      }
    }
    seal_yylval.symbol = inttable.add_string(s0);
    delete []s0;
	  return (CONST_INT);
  }
}

{OBJECT_CONST} {
  if (flag0 || flag2)
  {
    for (int i = 0; i < strlen(yytext); i ++)
    {
      s = strncat(s, yytext + i, 1);
      if (strlen(s) > 256)
      {
        flag0 = 0;
        flag2 = 0;
        delete []s;
        strcpy(seal_yylval.error_msg, "String constant too long");
        return (ERROR);
      }
    }
  }
  else if (!flag && !flag3)
  {
    seal_yylval.symbol = idtable.add_string(yytext);
	  return (OBJECTID);
  }
}

{type} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    seal_yylval.symbol = idtable.add_string(yytext);
    return (TYPEID);
  } 
}

{false_OBJECT_CONST} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    string s0 = "illegal TYPEID ";
    s0 += yytext;
    strcpy(seal_yylval.error_msg, s0.c_str());
	  return (ERROR);
  }
}

{false_ID} {
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
    string s0 = "illegal Identifier name ";
    s0 += yytext;
    strcpy(seal_yylval.error_msg, s0.c_str());
	  return (ERROR);
  }
}

.	{
  if (flag0 || flag2)
  {
    s = strcat(s, yytext);
    if (strlen(s) > 256)
    {
      flag0 = 0;
      flag2 = 0;
      delete []s;
      strcpy(seal_yylval.error_msg, "String constant too long");
      return (ERROR);
    }
  }
  else if (!flag && !flag3)
  {
	  strcpy(seal_yylval.error_msg, yytext); 
	  return (ERROR);
  }
}


<<EOF>> {
  if (flag)
  {
    flag = 0;
    strcpy(seal_yylval.error_msg, "EOF in comment");
    return (ERROR);
  }
  else if (flag0 || flag2)
  {
    flag0 = 0;
    flag2 = 0;
    strcpy(seal_yylval.error_msg, "EOF in string constant");
    return (ERROR);
  }
  else return 0;
}

%%