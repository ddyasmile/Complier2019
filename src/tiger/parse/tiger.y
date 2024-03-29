%filenames parser
%scanner tiger/lex/scanner.h
%baseclass-preinclude tiger/absyn/absyn.h

 /*
  * Please don't modify the lines above.
  */

%union {
  int ival;
  std::string* sval;
  S::Symbol *sym;
  A::Exp *exp;
  A::ExpList *explist;
  A::Var *var;
  A::DecList *declist;
  A::Dec *dec;
  A::EFieldList *efieldlist;
  A::EField *efield;
  A::NameAndTyList *tydeclist;
  A::NameAndTy *tydec;
  A::FieldList *fieldlist;
  A::Field *field;
  A::FunDecList *fundeclist;
  A::FunDec *fundec;
  A::Ty *ty;
  }

%token <sym> ID
%token <sval> STRING
%token <ival> INT

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  ARRAY  THEN ELSE TO END  
  BREAK NIL
  VAR 
  FUNCTION IF WHILE FOR TYPE OF DO LET IN


%right ASSIGN
%left OR
%left AND
%nonassoc EQ NEQ LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS

%type <exp> exp expseq
%type <explist> actuals  nonemptyactuals sequencing  sequencing_exps
%type <var>  lvalue one oneormore
%type <declist> decs decs_nonempty
%type <dec>  decs_nonempty_s vardec
%type <efieldlist> rec rec_nonempty
%type <efield> rec_one
%type <tydeclist> tydec
%type <tydec>  tydec_one
%type <field> tyfield
%type <fieldlist> tyfields tyfields_nonempty
%type <ty> ty
%type <fundeclist> fundec
%type <fundec> fundec_one

%start program


 /*
  * Put your codes here (lab3).
  */

%%
program:  exp  {absyn_root = $1;};

exp:      
        lvalue	                                {$$ = new A::VarExp(errormsg.tokPos, $1);}
        | NIL	        	                    {$$ = new A::NilExp(errormsg.tokPos);}
        | INT	                                {$$ = new A::IntExp(errormsg.tokPos, $1);}
        | STRING          	                    {$$ = new A::StringExp(errormsg.tokPos, $1);} 

	    | exp PLUS exp			                {$$ = new A::OpExp(errormsg.tokPos, A::PLUS_OP, $1, $3);}
        | exp MINUS exp      	   	            {$$ = new A::OpExp(errormsg.tokPos, A::MINUS_OP, $1, $3);}
        | exp TIMES exp   		                {$$ = new A::OpExp(errormsg.tokPos, A::TIMES_OP, $1, $3);}
        | exp DIVIDE exp  	   	                {$$ = new A::OpExp(errormsg.tokPos, A::DIVIDE_OP, $1, $3);}
        | exp EQ exp               	            {$$ = new A::OpExp(errormsg.tokPos, A::EQ_OP, $1, $3);}
        | exp NEQ exp    	                    {$$ = new A::OpExp(errormsg.tokPos, A::NEQ_OP, $1, $3);}
        | exp LT exp               	            {$$ = new A::OpExp(errormsg.tokPos, A::LT_OP, $1, $3);}
        | exp LE exp              	            {$$ = new A::OpExp(errormsg.tokPos, A::LE_OP, $1, $3);}
        | exp GT exp      	   	                {$$ = new A::OpExp(errormsg.tokPos, A::GT_OP, $1, $3);}
        | exp GE exp      	                    {$$ = new A::OpExp(errormsg.tokPos, A::GE_OP, $1, $3);}
	    | MINUS exp %prec UMINUS   	            {$$ = new A::OpExp(errormsg.tokPos, A::MINUS_OP, new A::IntExp(errormsg.tokPos, 0), $2);}

	    | ID LPAREN actuals RPAREN    	            {$$ = new A::CallExp(errormsg.tokPos, $1, $3);} 
        | ID LPAREN RPAREN         	            {$$ = new A::CallExp(errormsg.tokPos, $1, nullptr);}
        | ID LBRACE rec RBRACE                  {$$ = new A::RecordExp(errormsg.tokPos, $1, $3);}
	    | ID LBRACE RBRACE		                {$$ = new A::RecordExp(errormsg.tokPos, $1, nullptr);}
        | LPAREN sequencing RPAREN              {$$ = new A::SeqExp(errormsg.tokPos, $2);}
        | LPAREN RPAREN            	            {$$ = new A::VoidExp(errormsg.tokPos);}
        | LPAREN exp RPAREN                     {$$ = $2;}

        | lvalue ASSIGN exp  	   	            {$$ = new A::AssignExp(errormsg.tokPos, $1, $3);}
        | IF exp THEN exp ELSE exp 	            {$$ = new A::IfExp(errormsg.tokPos, $2, $4, $6);}
        | IF exp THEN exp		                {$$ = new A::IfExp(errormsg.tokPos, $2, $4, nullptr);}
        | WHILE exp DO exp         	            {$$ = new A::WhileExp(errormsg.tokPos, $2, $4);}
        | FOR ID ASSIGN exp TO exp DO exp	    {$$ = new A::ForExp(errormsg.tokPos, $2, $4, $6, $8);}
        | BREAK                    	            {$$ = new A::BreakExp(errormsg.tokPos);}
        | LET decs IN expseq END   	            {$$ = new A::LetExp(errormsg.tokPos, $2, $4);} 
        | ID LBRACK exp RBRACK OF exp           {$$ = new A::ArrayExp(errormsg.tokPos, $1, $3, $6);}

	    | exp AND exp           	            {$$ = new A::IfExp(errormsg.tokPos, $1, $3, new A::IntExp(errormsg.tokPos, 0));}
        | exp OR exp               	            {$$ = new A::IfExp(errormsg.tokPos, $1, new A::IntExp(errormsg.tokPos, 1), $3);}
	;

expseq:
        sequencing                              {$$ = new A::SeqExp(errormsg.tokPos, $1);}
    ;

sequencing:
                                                {$$ = nullptr;}
        | exp                                   {$$ = new A::ExpList($1, nullptr);}
        | sequencing_exps                       {$$ = $1;}
    ;

sequencing_exps:
        exp SEMICOLON exp                       {$$ = new A::ExpList($1, new A::ExpList($3, nullptr));}
        | exp SEMICOLON sequencing_exps	   	    {$$ = new A::ExpList($1, $3);}
    ;

actuals:
                                                {$$ = nullptr;}
        | nonemptyactuals                       {$$ = $1;}
    ;

nonemptyactuals:
        exp                                     {$$ = new A::ExpList($1, nullptr);}
        | exp COMMA nonemptyactuals	   	        {$$ = new A::ExpList($1, $3);}
    ;

rec_one:
        ID EQ exp                               {$$ = new A::EField($1, $3);}
    ;

rec:
                                                {$$ = nullptr;}
        | rec_nonempty                          {$$ = $1;}
    ;

rec_nonempty:
        rec_one                                 {$$ = new A::EFieldList($1, nullptr);}
        | rec_one COMMA rec_nonempty            {$$ = new A::EFieldList($1, $3);}
    ;

tyfield: 
        ID COLON ID               	            {$$ = new A::Field(errormsg.tokPos, $1, $3);} 
    ;

tyfields:
                                                {$$ = nullptr;}
        | tyfields_nonempty                     {$$ = $1;}
    ;

tyfields_nonempty:
        tyfield                                 {$$ = new A::FieldList($1, nullptr);}
        | tyfield COMMA tyfields_nonempty       {$$ = new A::FieldList($1, $3);}
    ;

ty:
        ID                                      {$$ = new A::NameTy(errormsg.tokPos, $1);}
        | LBRACE tyfields RBRACE   	            {$$ = new A::RecordTy(errormsg.tokPos, $2);}
        | ARRAY OF ID              	            {$$ = new A::ArrayTy(errormsg.tokPos, $3);}
    ;

tydec_one:
        TYPE ID EQ ty			                {$$ = new A::NameAndTy($2, $4);}
    ;

tydec:
        tydec_one                               {$$ = new A::NameAndTyList($1, nullptr);}
        | tydec_one tydec                       {$$ = new A::NameAndTyList($1, $2);}
    ;

vardec:  
        VAR ID ASSIGN exp                       {$$ = new A::VarDec(errormsg.tokPos, $2, nullptr, $4);}
        | VAR ID COLON ID ASSIGN exp            {$$ = new A::VarDec(errormsg.tokPos, $2, $4, $6);}
    ;

fundec_one:
        FUNCTION ID LPAREN tyfields RPAREN EQ exp  	            {$$ = new A::FunDec(errormsg.tokPos, $2, $4, NULL, $7);}
	    | FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp    {$$ = new A::FunDec(errormsg.tokPos, $2, $4, $7, $9);}
    ;

fundec:
        fundec_one                              {$$ = new A::FunDecList($1, nullptr);}
        | fundec_one fundec                     {$$ = new A::FunDecList($1, $2);}
    ;

decs_nonempty_s:
        vardec                                  {$$ = $1;}
        | tydec                 		        {$$ = new A::TypeDec(errormsg.tokPos, $1);}
        | fundec                                {$$ = new A::FunctionDec(errormsg.tokPos, $1);}
    ;

decs:
        decs_nonempty_s                         {$$ = new A::DecList($1, nullptr);}
        | decs_nonempty_s decs                  {$$ = new A::DecList($1, $2);}
    ;

lvalue:
        ID                                      {$$ = new A::SimpleVar(errormsg.tokPos, $1);}
        | oneormore			                    {$$ = $1;}
    ;

one:
        ID DOT ID                               {$$ = new A::FieldVar(errormsg.tokPos, new A::SimpleVar(errormsg.tokPos, $1), $3);}
        | ID LBRACK exp RBRACK                  {$$ = new A::SubscriptVar(errormsg.tokPos, new A::SimpleVar(errormsg.tokPos, $1), $3);}
    ;

oneormore:
        one                                     {$$ = $1;}
        | oneormore DOT ID		                {$$ = new A::FieldVar(errormsg.tokPos, $1, $3);}
	    | oneormore LBRACK exp RBRACK           {$$ = new A::SubscriptVar(errormsg.tokPos, $1, $3);}
	;
