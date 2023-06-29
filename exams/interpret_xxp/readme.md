CZ:
Naprogramujte interpret jednoduchého procedurálního jazyka XXP.

Příklad programu pro výpočet faktoriálu v XXP:

; factorial
PAR=10
COUNTER=1
FACT=1           ; result
; loop
FACT = FACT * COUNTER
COUNTER=COUNTER+1
TMP = PAR - COUNTER
TMP ? 5         ; if( PAR != COUNTER) goto loop
; end

Zdrojový kód je řádkově orientovaný, jeden příkaz na jedné řádce. Vše od znaku ; (středník) do konce řádky se ignoruje. Všechny bílé znaky (mezera, tab) se ignorují.

Řádky jsou číslovány od 0, do číslování se započítávají všechny řádky včetně prázdých.

Číslo je posloupnost číslic. Všechna čísla jsou celočíselná, předpokládejte, že se vejdou do int, testy na přetečení nejsou nutné.

Identifikátor proměnné je posloupnost písmen, proměnné jsou case-sensitive. Proměnná může obsahovat pouze celé číslo, neinicializovaná proměnná má hodnotu 0. Příkaz je buď přiřazení nebo podmíněný skok.

Přiřadit proměnné lze buď hodnotu (tj. číslo nebo obsah jiné proměnné) nebo dvě hodnoty spojené operátorem. Složitější výrazy nejsou podporovány. Příkaz skoku otestuje hodnotu před znakem ?. V případě, že je nenulová, jako další se provede řádka s číslem odpovídajícím hodnotě za znakem ?. Pokud je hodnota nulová, program pokračuje následujícím řádkem.

Syntax:
<Number> ::= {Digit}*
<Identifier> ::= {Letter}*
<Value> ::= <Number> | <Identifier>
<Operator> ::= '+' | '-' | '*' | '/'

Assignment:
<Identifier> = <Value>
<Identifier> = <Value> <Operator> <Value>

Jump:
<Value> ? <Value>

Naprogramujte interpret jazyka XXP, který načte zdrojový kód ze souboru uvedeného jako jediný parametr příkazové řádky nebo ze standardního vstupu, pokud program žádné parametry nemá. Interpretace začíná na řádku 0 a končí tehdy, pokud by měla být prováděna neexistující řádka (korektní běh), anebo v případě výskytu jakékoliv chyby (chybový běh).

V případě korektního běhu interpret vypíše v lexikografickém uspořádání název a obsah všech definovaných proměnných ve tvaru

Hello=20
X=17

na každé řádce jedna proměnná.

V případě chybového běhu program vypíše na standardní výstup text error a (za mezerou) číslo řádky, na které byla chyba detekována. Za chybu je považován pokus o interpretaci jakékoliv řádky, která nevyhovuje výše uvedené syntaxi. Je nutné detekovat jakékoliv odchylky od syntaxe interpretovaných řádek. Řádky, které se neprovádí, není nutné kontrolovat. V případě výskytu chyby před spuštěním interpretace první řádky program vypíše error 0.

Pro výše uvedený program interpret vypíše:

COUNTER=10
FACT=362880
PAR=10
TMP=0

Hodnotící kritéria: funkčnost, stabilita, efektivita, dekompozice, rozhraní, čitelnost a elegance zdrojových kódů.

Důkladně otestujte, obzvláštní péči věnujte ošetření syntakticky nesprávného kódu.

Testy 01-03 obashují syntakticky korektní XXP kód

Testy 04-12 obashují kód se syntaktickými chybami

Přiložené soubory (sXX.xxp):

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088be525-6b83-11eb-a1a9-005056ad4f31/download

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088b4bab-6b83-11eb-a1a9-005056ad4f31/download

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088ab015-6b83-11eb-a1a9-005056ad4f31/download
EN:
Program the interpreter of the simple procedural language XXP.

Example of a program for calculating the factorial in XXP:

; factorial
PAR=10
COUNTER=1
FACT=1           ; result
; loop
FACT = FACT * COUNTER
COUNTER=COUNTER+1
TMP = PAR - COUNTER
TMP ? 5         ; if( PAR != COUNTER) goto loop
; end

The source code is line-oriented, one statement per line. Everything from the ; (semicolon) to the end of the line is ignored. All whitespace (space, tab) is ignored.

Lines are numbered from 0; all lines, including blank ones, are included in the numbering.

A number is a sequence of digits. All numbers are integers, assume they fit in the int, no overflow tests are required.

The variable identifier is a sequence of letters, the variables are case-sensitive. The variable can only contain an integer, the uninitialized variable has the value 0.

The command is either an assignment or a conditional jump.

A variable can be assigned by either a value (that is, a number or the contents of another variable) or by two values joined by an operator. More complex expressions are not supported.

The jump statement tests the value before the ? character. If it is non-zero, the next line is a line with a number corresponding to the value after the ? character. If the value is zero, the program continues with the next line.

Syntax:
<Number> ::= {Digit}*
<Identifier> ::= {Letter}*
<Value> ::= <Number> | <Identifier>
<Operator> ::= '+' | '-' | '*' | '/'

Assignment:
<Identifier> = <Value>
<Identifier> = <Value> <Operator> <Value>

Jump:
<Value> ? <Value>

Program an XXP interpreter that reads the source code from a file specified as a single command line parameter or from the standard input if the program has no parameters. Interpretation starts at line 0 and ends if a non-existent line should be executed (correct run) or if any error occurs (error run).

In the case of a correct run, the interpreter prints (sorted lexicographically) the name and content of all defined variables:

Hello=20
X=17

one variable per line.

In case of an error run, the program writes to the standard output the text error and the number of the line on which the error was detected. An attempt to interpret any line that does not conform to the above syntax is considered an error. It is necessary to detect any syntax error of the interpreted lines. Lines that are not executed do not need to be checked. If an error occurs before starting the interpretation of the first line, the program prints error 0.

For the above program, the iterpreter will output:

COUNTER=10
FACT=362880
PAR=10
TMP=0

Aim for working, stable, efficient, and readable code.

Use enough test data, especially for syntactically malformed XXP code.

Tests 01-03 use syntactically correct XXP code.

Tests 04-12 contain syntax errors in the code.

Attached files:

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088be525-6b83-11eb-a1a9-005056ad4f31/download

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088b4bab-6b83-11eb-a1a9-005056ad4f31/download

https://recodex.mff.cuni.cz/api/v1/uploaded-files/088ab015-6b83-11eb-a1a9-005056ad4f31/download

