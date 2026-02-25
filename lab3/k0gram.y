/* ========== KEYWORDS ========== */
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

/* ========== LITERALS ========== */
%token INTEGERLITERAL
%token LONGLITERAL
%token REALLITERAL
%token DOUBLELITERAL
%token BOOLEANLITERAL
%token CHARACTERLITERAL
%token STRINGLITERAL
%token NULLLITERAL

/* ========== IDENTIFIERS ========== */
%token IDENT

/* ========== ASSIGNMENT OPERATORS ========== */
%token ASSIGNMENT
%token ADD_ASSIGN
%token SUB_ASSIGN
%token MUL_ASSIGN
%token DIV_ASSIGN
%token MOD_ASSIGN

/* ========== ARITHMETIC OPERATORS ========== */
%token ADD
%token SUB
%token MUL
%token DIV
%token MOD

/* ========== INCREMENT/DECREMENT ========== */
%token INCR
%token DECR

/* ========== COMPARISON OPERATORS ========== */
%token EQ
%token NEQ
%token GTE
%token LTE

/* ========== REFERENTIAL EQUALITY ========== */
%token REF_EQ
%token REF_NEQ

/* ========== LOGICAL OPERATORS ========== */
%token AND
%token OR
%token NOT

/* ========== NULL-SAFE OPERATORS ========== */
%token SAFE_CALL
%token ELVIS
%token QUEST

/* ========== RANGE OPERATORS ========== */
%token RANGE
%token RANGE_UNTIL

/* ========== TYPE CAST ========== */
%token AS

/* ========== DELIMITERS & PUNCTUATION ========== */
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

/* ========== SPECIAL ========== */
%token NEWLINE

%%