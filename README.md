# Lexer_Parser
## Project 1: Lexical Analysis
Here is the list of tokens the lexer supports:    
IF WHILE DO THEN PRINT  
PLUS MINUS DIV MULT EQUAL  
COLON COMMA SEMICOLON LBRACE RBRACE LPAREN RPAREN  
NOTEQUAL GREATER LESS LTEQ GTEQ  
DOT NUM ID REALNUM BASE08NUM BASE16NUM  

## Project 2: Parsing
Here is the grammar for the input language:  
program -> global_vars scope  
global_vars -> epsilon  
global_vars -> var_list SEMICOLON  
var_list -> ID  
var_list -> ID COMMA var_list  
scope -> ID LBRACE public_vars private_vars stmt_list RBRACE  
public_vars -> epsilon  
public_vars -> PUBLIC COLON var_list SEMICOLON  
private_vars -> epsilon  
private_vars -> PRIVATE COLON var_list SEMICOLON  
stmt_list -> stmt  
stmt_list -> stmt stmt_list  
stmt -> ID EQUAL ID SEMICOLON  
stmt -> scope  

## Project 3: Type Checking
Here is the grammar for the input language:  
program -> global_vars body  
global_vars -> epsilon  
global_vars -> var_decl_list  
var_decl_list -> var_decl  
var_decl_list -> var_decl var_decl_list  
var_decl -> var_list COLON type_name SEMICOLON  
var_list -> ID  
var_list -> ID COMMA var_list  
type_name -> INT  
type_name -> REAL  
type_name -> BOOL  
body -> LBRACE stmt_list RBRACE  
stmt_list -> stmt  
stmt_list -> stmt stmt_list  
stmt -> assignment_stmt  
stmt -> if_stmt  
stmt -> while_stmt  
stmt -> switch_stmt  
assignment_stmt -> ID EQUAL expression SEMICOLON  
expression -> primary  
expression -> binary_operator expression expression  
expression -> unary_operator expression  
unary_operator -> NOT  
binary_operator -> PLUS | MINUS | MULT | DIV  
binary_operator -> GREATER | LESS | GTEQ | LTEQ | EQUAL | NOTEQUAL  
primary -> ID  
primary -> NUM  
primary -> REALNUM  
primary -> TRUE  
primary -> FALSE  
if_stmt -> IF LPAREN expression RPAREN body  
while_stmt -> WHILE LPAREN expression RPAREN body  
switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE  
case_list -> case  
case_list -> case case_list  
case -> CASE NUM COLON body  
