CZ:
Vaším úkolem je implementovat program, který spočítá minimum a maximum celočíselné funkce celočíselné proměnné na daném intervalu.

Funkce je zadána jako postfixový výraz obsahující proměnnou x, celočíselné konstanty bez znaménka a celočíselné operátory +,-,*,/. Příklad:

14 x 71 /+3000 x-75/*

je korektní postfixový výraz, odpovídající výrazu

(14+(x/71))*((3000-x)/75)

v jazyce C.

Postfixový výraz je zapsán v souboru, jehož jméno je prvním parametrem programu (argv[1]). Výraz může obsahovat white-space, včetně oddělovače řádků. (Pokud jsou vedle sebe dvě čísla a/nebo proměnné, musí mezi nimi být white-space.)

Další dva parametry (argv[2], argv[3]) jsou dekadické zápisy celých čísel (se znaménkem), které určují meze (uzavřeného) intervalu. Úkolem programu je vypočítat hodnotu výrazu pro všecha celá čísla z tohoto intervalu a zaznamenat minimální a maximální dosaženou hodnotu.

Spočtené minimum a maximum vypište na standardní výstup ve tvaru

min=-20 max=150

Veškerá aritmetika se odehrává v 32-bitovém celočíselném typu se znaménkem, např. std::int_least32_t. Implementace by měla být dostatečně efektivní, aby (v Release režimu překladu) dokázala vyhodnotit přiměřeně velký výraz pro milion hodnot proměnné během několika (desítek) sekund. Výraz ze souboru je tedy nutné konvertovat do vhodné vnitřní reprezentace umožňující rychlé vyhodnocování.

Dbejte na vhodnou dekompozici problému do funkcí a/nebo objektů. Je vhodné oddělit syntaktickou analýzu výrazu, jeho vyhodnocování a hledání minima/maxima, nejlépe tak, aby příslušné funkce byly použitelné i v jiném prostředí.

Nekorektní vstupy (nezadané parametry, neexistující soubor, nekorektní výraz apod.) by neměly způsobit nekontrolované chování programu. Jestliže pro některé hodnoty x výraz obsahuje dělení nulou, tyto hodnoty x mají být tiše ignorovány při výpočtu minima a maxima.

Hodnocení

Řešení bude oceněno 0 až 15 body. Tyto body ovlivní získání zápočtu i známku ze zkoušky. Hodnotí se funkčnost, použité datové struktury a algoritmy, čitelnost zdrojového kódu a dodržení pravidel a doporučení jazyka C++. Rychlost běhu programu se přímo nehodnotí, ale výrazně neefektivní řešení budou penalizována.

Nápověda

Vzorové řešení má 193 řádek a používá tyto typy, funkce a proměnné ze standardní knihovny:

<fstream>: ifstream
<istream>: istream
<string>: string
<cctype>: isdigit
<cstdlib>: strtol
<cstdint>: int_least32_t
<vector>: vector
<memory>: unique_ptr, make_unique
<functional>: plus, minus, multiplies, divides
<limits>: numeric_limits
<algorithm>: min, max
<stdexcept>: range_error
<iostream>: cout, cerr
EN:
Your task is to implement a program to compute the minimal and the maximal value of an integer function of an integer variable over a given interval.

The function is specified as a post-fix expression containing the variable x, non-negative integral constants, and integer operators +,-,*,/. Example:

14 x 71 /+3000
x-75/*

is a correct post-fix expression corresponding to the C-language expression

(14+(x/71))*((3000-x)/75)

The post-fix expression is located in a file; the name of the file is specified as the first argument (argv[1]) of the program. The expression may contain white-space, including line separators \n. (If numbers and/or variables are adjacent, at least one whitespace must be present.)

The following two arguments (argv[2], argv[3]) are (possibly signed) integral numbers in decimal notation which determine the bounds of an interval. The program shall compute the values of the expression for all the integer numbers in this interval (including the bounds) and determine the minimum and the maximum of these values.

The results shall be written to the standard output in the following format:

min=-20 max=150

Expression evaluation shall be carried in 32-bit signed integers, e.g. std::int_least32_t. Your implementation must be sufficiently fast: it must be able (in Release mode) to evaluate an expression for a million variable values in few seconds. Thus, the expression must be converted into suitable internal representation.

Apply suitable problem decomposition into functions and/or objects. Try to separate parsing, evaluation, and min/max determination so that the respective functions could be reused in a different task.

Incorrect inputs (missing program arguments, non-existent file, incorrect expression etc.) shall not lead to uncontrolled program behavior. If, for some values of x, the evaluation leads to division by zero, such values of x shall be silently ignored when calculating min/max.

Points

The recodex system will award up to 15 points depending on the success rate of the tests. From the points awarded by recodex, some points may be subtracted as a penalty for source-code imperfections. Good code shall use appropriate data structures and algorithms, shall be well-readable, and shall adhere to rules, conventions, and recommendations of the C++ language. Points may also be deducted for approaches that result in significant degradation of performance (e.g. storing constants as strings and repeatedly converting them during evaluation). In addition, extremaly slow program may not finish within recodex time limits.

Hints

You may need the following types, functions, and variables of the standard library:

<fstream>: ifstream
<istream>: istream
<string>: string
<cctype>: isdigit
<cstdlib>: strtol
<cstdint>: int_least32_t
<vector>: vector
<memory>: unique_ptr, make_unique
<functional>: plus, minus, multiplies, divides
<limits>: numeric_limits
<algorithm>: min, max
<stdexcept>: range_error
<iostream>: cout, cerr


