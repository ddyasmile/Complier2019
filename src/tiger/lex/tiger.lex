%filenames = "scanner"

 /*
  * Please don't modify the lines above.
  */

 /* You can add lex definitions here. */

%x COMMENT STR

%%

 /*
  * TODO: Put your codes here (lab2).
  *
  * Below is examples, which you can wipe out
  * and write regular expressions and actions of your own.
  *
  * All the tokens:
  *   Parser::ID
  *   Parser::STRING
  *   Parser::INT
  *   Parser::COMMA
  *   Parser::COLON
  *   Parser::SEMICOLON
  *   Parser::LPAREN
  *   Parser::RPAREN
  *   Parser::LBRACK
  *   Parser::RBRACK
  *   Parser::LBRACE
  *   Parser::RBRACE
  *   Parser::DOT
  *   Parser::PLUS
  *   Parser::MINUS
  *   Parser::TIMES
  *   Parser::DIVIDE
  *   Parser::EQ
  *   Parser::NEQ
  *   Parser::LT
  *   Parser::LE
  *   Parser::GT
  *   Parser::GE
  *   Parser::AND
  *   Parser::OR
  *   Parser::ASSIGN
  *   Parser::ARRAY
  *   Parser::IF
  *   Parser::THEN
  *   Parser::ELSE
  *   Parser::WHILE
  *   Parser::FOR
  *   Parser::TO
  *   Parser::DO
  *   Parser::LET
  *   Parser::IN
  *   Parser::END
  *   Parser::OF
  *   Parser::BREAK
  *   Parser::NIL
  *   Parser::FUNCTION
  *   Parser::VAR
  *   Parser::TYPE
  */

<INITIAL>"\n"                        {adjust(); errormsg.Newline();}

<INITIAL>","                         {adjust();return Parser::COMMA;}
<INITIAL>":"                         {adjust();return Parser::COLON;}
<INITIAL>";"                         {adjust();return Parser::SEMICOLON;}
<INITIAL>"{"                         {adjust();return Parser::LBRACE;}
<INITIAL>"}"                         {adjust();return Parser::RBRACE;}
<INITIAL>"("                         {adjust();return Parser::LPAREN;}
<INITIAL>")"                         {adjust();return Parser::RPAREN;}
<INITIAL>"["                         {adjust();return Parser::LBRACK;}
<INITIAL>"]"                         {adjust();return Parser::RBRACK;}
<INITIAL>"."                         {adjust();return Parser::DOT;}
<INITIAL>"+"                         {adjust();return Parser::PLUS;}
<INITIAL>"-"                         {adjust();return Parser::MINUS;}
<INITIAL>"*"                         {adjust();return Parser::TIMES;}
<INITIAL>"/"                         {adjust();return Parser::DIVIDE;}
<INITIAL>"="                         {adjust();return Parser::EQ;}
<INITIAL>"<>"                        {adjust();return Parser::NEQ;}
<INITIAL>"<"                         {adjust();return Parser::LT;}
<INITIAL>"<="                        {adjust();return Parser::LE;}
<INITIAL>">"                         {adjust();return Parser::GT;}
<INITIAL>">="                        {adjust();return Parser::GE;}
<INITIAL>"&"                         {adjust();return Parser::AND;}
<INITIAL>"|"                         {adjust();return Parser::OR;}
<INITIAL>":="                        {adjust();return Parser::ASSIGN;}

<INITIAL>array                       {adjust();return Parser::ARRAY;}
<INITIAL>if                          {adjust();return Parser::IF;}
<INITIAL>then                        {adjust();return Parser::THEN;}
<INITIAL>else                        {adjust();return Parser::ELSE;}
<INITIAL>while                       {adjust();return Parser::WHILE;}
<INITIAL>for                         {adjust();return Parser::FOR;}
<INITIAL>to                          {adjust();return Parser::TO;}
<INITIAL>do                          {adjust();return Parser::DO;}
<INITIAL>let                         {adjust();return Parser::LET;}
<INITIAL>in                          {adjust();return Parser::IN;}
<INITIAL>end                         {adjust();return Parser::END;}
<INITIAL>of                          {adjust();return Parser::OF;} 
<INITIAL>break                       {adjust();return Parser::BREAK;}
<INITIAL>nil                         {adjust();return Parser::NIL;}
<INITIAL>function                    {adjust();return Parser::FUNCTION;}
<INITIAL>var                         {adjust();return Parser::VAR;}
<INITIAL>type                        {adjust();return Parser::TYPE;}

<INITIAL>[a-zA-Z][a-zA-Z0-9_]*       {adjust();return Parser::ID;}
<INITIAL>[0-9]+                      {adjust();return Parser::INT;}
<INITIAL>[ \t]+                      {adjust();}

<INITIAL>"/*"                        {adjust();commentLevel_=1;begin(StartCondition__::COMMENT);}
<INITIAL>\"                          {adjust();begin(StartCondition__::STR);}

<INITIAL>.                           {adjust(); errormsg.Error(errormsg.tokPos, "illegal token");}


<STR>\"                              {adjustStr(0);begin(StartCondition__::INITIAL);return Parser::STRING;}
<STR>"\\n"                           {adjustStr(1);}
<STR>"\\t"                           {adjustStr(2);}
<STR>"\\\""                          {adjustStr(3);}
<STR>"\\\\"                          {adjustStr(4);}
<STR>\\[0-9][0-9][0-9]               {adjustStr(5);}
<STR>\\[ \n\t\f]+\\                  {adjustStr(6);}
<STR>\\\^@	                         {adjustStr(7);}
<STR>\\\^[A-Z]                       {adjustStr(8);}
<STR>.                               {adjustStr(9);}

<COMMENT>"*/"                        {adjust();commentLevel_--;
                                     if(commentLevel_==0)begin(StartCondition__::INITIAL);}
<COMMENT>"/*"                        {adjust();commentLevel_++;}
<COMMENT>"\n"                        {adjust();errormsg.Newline();}
<COMMENT>.                           {adjust();}

