```
program      → declaration* EOF
declaration  → func_decl | var_decl
func_decl    → type IDENTIFIER '(' params? ')' block
var_decl     → type IDENTIFIER ('=' expression)? ';'
type         → 'int' | 'void'
params       → param (',' param)*
param        → type IDENTIFIER
block        → '{' statement* '}'
statement    → var_decl | if_stmt | return_stmt | print_stmt | expr_stmt | block
if_stmt      → 'if' '(' expression ')' statement ('else' statement)?
return_stmt  → 'return' expression? ';'
print_stmt   → 'print' expression ';'
expr_stmt    → expression ';'
expression   → assignment
assignment   → IDENTIFIER '=' assignment | equality
equality     → comparison (('==' | '!=') comparison)*
comparison   → term (('<' | '>') term)*
term         → unary (('+' | '-') unary)*
unary        → '!' unary | primary
primary      → NUMBER | IDENTIFIER | '(' expression ')' | call
call         → IDENTIFIER '(' arguments? ')'
arguments    → expression (',' expression)*
```