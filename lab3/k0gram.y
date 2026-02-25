//tokens
%token FUN
%token VAL
%token VAR
%token IF
%token ELSE
%token WHILE
%token FOR
%token RETURN
%token BREAK
%token CONTINUE
%token WHEN
%token IN
%token DO
%token IMPORT
%token CONST

//the literals
%token INTEGERLITERAL
%token LONGLITERAL
%token REALLITERAL
%token DOUBLELITERAL
%token BOOLEANLITERAL
%token CHARACTERLITERAL
%token STRINGLITERAL
%token NULLLITERAL

//identifiers 
%token IDENT

//assignments
%token ASSIGNMENT
%token ADD_ASSIGN
%token SUB_ASSIGN
%token MUL_ASSIGN
%token DIV_ASSIGN
%token MOD_ASSIGN

//arithmetic
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD


%token INCR
%token DECR


%token EQ
%token NEQ
%token GTE
%token LTE


%token REF_EQ
%token REF_NEQ


%token AND
%token OR
%token NOT


%token SAFE_CALL
%token ELVIS
%token QUEST


%token RANGE
%token RANGE_UNTIL


%token AS


%token LPAREN
%token RPAREN
%token LCURL
%token RCURL
%token LSQUARE
%token RSQUARE
%token LANGLE
%token RANGLE
%token COMMA
%token DOT
%token COLON
%token SEMI
%token ARROW
%token DOUBLE_COLON

%%
declaration:
	functionDeclaration
	| assignment
	;

statements:
	statement semis
	| statement semis statements
	| SEMICOLON
	;

semis:
	SEMICOLON semis
	| SEMICOLON
	;

statement:
	declaration
	| loopStatement
	| expression
	;

assignment:
	IDENTIFIER ASSIGNMENT expression
	| IDENTIFIER ADD_ASSIGNMENT expression
	| IDENTIFIER SUB_ASSIGNMENT expression
	| arrayIndex ASSIGNMENT expression
	| arrayIndex ADD_ASSIGNMENT expression
	| arrayIndex SUB_ASSIGNMENT expression
	;

arrayIndex:
	IDENTIFER LSQUARE INTEGER_LITERAL RSQUARE
	;

loopStatement:
	forstatement
	| whileStatement
	| doWhileStatement
	;


expression:
	disjunction
	;

disjunction:
	disjunction DISJ conjunction
	| conjunction
	;

conjunction:
	conjunction CONJ equality
	| equality
	;

controlStructureBody:
	block
	| statement SEMICOLON
	;
%%
