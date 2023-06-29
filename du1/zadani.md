CZ:
Bezpečné vektory
Testovací zdrojové soubory

    test_01.cpp
    test_02.cpp
    test_03.cpp Rozdělen do 5 testů v Recodexu
    test_util.hpp Použit v každém testu

Motivace

Silně typovaný systém indexů, produkující překladovou chybu, pokud je jako index do pole použita nesprávná proměnná:

for (auto i : pears.range())
{
  std::cout << pears[i];  // CORRECT
  std::cout << apples[i]; // COMPILATION ERROR
}

Tohoto cíle je dosaženo pomocí uživatelem definovaných tagů připojených jako argument šablony ke každému typu reprezentujícímu kontejner, rozsah jeho indexů nebo jednotlivý index:

vector_class<double,pear_tag> pears;
vector_class<double,apple_tag> apples;

Tag je uživatelem definovaná struktura, která je potomkem struktury tag_base, což umožňuje testování jejich správného použití pomocí konceptu tag.

Jako bezpečnostní omezení jsou na tagovaných indexech znemožněny aritmetické operace. Tím jsou znemožněny i cykly ve tvaru (;i<n;i++). Místo nich je nutné používat range-based for, a tedy používat objekty reprezentující intervaly (range) příslušně tagovaných hodnot indexů. V mnoha případech bude několik kontejnerů používat tentýž rozsah:

assert(a.range() == b.range()); // implies that a and b must have the same tag
for (auto i : a.range())
{
   c += a[i] * b[i];
}

Kromě bezpečnosti systém také umožňuje redukovat velikost typů reprezentujících indexy pro malé kontejnery, v porovnání se standardně používanými typy std::size_t a std::ptrdiff_t. Požadovaný celočíselný typ může být specifikován jako typový prvek struktury tagu:

struct rare_tag : tag_base {
  using carrier_type = std::uint_least16_t;
}
std::vector<index_class<rare_tag>> indexes;  // 4-times smaller than with std::size_t

Součásti k implementaci

Implementace musí být vložena do recodexu jako hlavičkový soubor tagged_vector.hpp.
tag_base

Prázdná struktura tag_base.
tag

Koncept tag reprezentující struktury/třídy, které dědí tag_base.
index_carrier_type

template<tag T> using index_carrier_type = /*...*/;

Typ index_carrier_type<T> je celočíselný typ použitý pro implementaci index_class<T> a range_class<T>. index_carrier_type<T> má být roven T::carrier_type, pokud je tento definován. Pokud T::carrier_type není definován, index_carrier_type<T> má být roven std::size_t.
vector_class

template<typename E, tag T> class vector_class;

Třída vector_class<E,T> reprezentuje pole dynamicky definované velikosti obsahující prvky typu E. Rozhraní třídy vector_class<E,T> je v principu podmnožinou rozhraní třídy std::vector<E>, podporující následující operace (s mírně odlišnými argumenty):

    konstruktor bez parametrů produkuje prázdný kontejner s rozsahem indexů [0,0)
    explicitní konstruktor s dvěma parametry typů range_class<T> a const E& určujícími rozsah platných indexů a iniciální hodnotu všech prvků; druhý parametr má defaultní hodnotu E()
    copy- a move- konstruktory a přiřazovací operátory (hodnota zdrojového operandu po provedení move operace má být shodná s hodnotou produkovanou konstruktorem bez parametrů)
    operátor [] pro přístup k prvkům kontejneru, akceptující operand typu index_class<T> (operátor musí vracet referencí a respektovat pravidla logické konstantnosti obvyklá u kontejnerů)
    metoda range() vracející range_class<T> popisující rozsah platných indexů

Poznámka: Narozdíl od třídy std::vector třída vector_class nepodporuje iterátory, přidávání/odebírání prvků (počet prvků existujícího kontejneru lze změnit pouze přiřazovacím operátorem), a řadu méně významných operací jako back(), data() apod. Metoda range() slouží jako náhrada metody size().

Poznámka: Narozdíl od std::vector třída vector_class podporuje indexování libovolným intervalem (b,...,e-1), tedy nejen intervalem začínajicím v nule. Tato čísla však nemohou být přímo argumentem operátoru [], ale musí být zapouzdřena uvnitř třídy index_class<T>. Následkem toho budou indexy nejčastěji generovány cyklem řízeným objektem typu range_class<T>, se stejným rozsahem jako byl použit při vytváření kontejneru. Takový objekt je možné získat voláním metody range().
index_class

template<tag T> class index_class;

Třída index_class<T> zapouzdřuje číslo typu index_carrier_type<T> sloužící jako index pro přístup k prvkům kontejneru vector_class<E,T> (s tímtéž tagem T a libovolným typem elementu E).

Hlavním účelem třídy index_class<T> je logicky spojit tag T s proměnnou obsahující index a zajistit, že index_class<T1> nebude kompatibilní s vector_class<E,T2>::operator[] pokud tag T2 je odlišný od T1.

index_class<T> má podporovat tyto operace:

    konstruktor bez parametrů, produkující takový obsah, který způsobí výjimku std::range_error("Uninitialized index") při použití tohoto objektu jako parametr pro vector_class<E,T>::operator[] nebo pro některou z níže uvedených operací (jediná povolená akce v takovém stavu je použití na levé straně přiřazení)
    copy-constructor a copy-assignment
    explicitní konstruktor akceptující parametr typu index_carrier_type<T> pro inicializaci vnitřní hodnoty
    metoda value() vracející vnitřní hodnotu typu index_carrier_type<T>
    operátory == a != (pro dva objekty typu index_class se stejným tagem)
    formátovaný výstup do std::ostream pomocí operátoru << (se stejným efektem jako použití << pro vnitřní číselnou hodnotu)

range_class

template<tag T> class range_class;

Třída range_class<T> má podporovat:

    konstruktor akceptující dva argumenty (ib, ie) typu index_carrier_type<T>
    explicitní konstruktor akceptující argument (ie) typu index_carrier_type<T>, ekvivalent použití konstruktoru s parametry (0,ie)
    copy-constructor a copy-assignment
    operátory == a != (pro dva objekty typu range_class se stejným tagem)
    metody begin() a end() podobně jako v std::ranges::range

Objekt typu range_class reprezentuje polouzavřený interval [ib,ie) celých čísel. Funkce begin a end mají vracet iterátor kategorie bidirectional, zajišťující přístup k fiktivnímu kontejneru s prvky typu index_class<T> s hodnotami {ib, ib+1, ib+2, ..., ie-1}. Tento seznam hodnot nesmí být nikde uložen, t.j. všechny metody třídy range_class<T> stejně jako všechny operace s iterátory vracenými metodami begin() a end() musí pracovat v konstantním čase a konstantní paměti.

Poznmámka: Přítomnost metod begin a end umožňuje použití range_class<T> v range-based for-loops. range_class<T> ale nemusí splňovat všechna kritéria konceptu std::ranges::range.

Poznámka: Účel třídy range_class<T> je podobný jako u std::ranges::iota_view až na to, že čísla generovaná objektem typu range_class<T> jsou zapouzdřená v objektech typu index_class<T>.
Iterátory

Iterátory vracené metodami range_class<T>::begin() a range_class<T>::end() musejí splňovat požadavky kladené na iterátor kategorie bidirectional nad fiktivním kontejnerem, t.j. podporovat následující prvky a operace:

    vnořený typ iterator_category rovný std::bidirectional_iterator_tag
    vnořený typ value_type rovný index_class<T>
    vnořený typ reference rovný index_class<T>
    vnořený typ difference_type rovný znaménkové verzi typu index_carrier_type<T>
    konstruktor bez parametrů (produkující neplatný iterátor; diagnostika není vyžadována)
    copy-constructor a copy-assignment
    *it vracející hodnotu index_class<T>
    ++it, it++, --it, it--
    it1==it2, it1!=it2

Příklad
Tagy

struct pear_tag : tag_base {};
struct apple_tag : tag_base {
  using carrier_type = std::ptrdiff_t;  // use signed type
};

Intervaly

range_class<pear_tag> pear_range(1, 10);
range_class<apple_tag> apple_range(-20, 20);

Kontejnery

vector_class<double, pear_tag> pear_weights(pear_range, 3.14);
vector_class<double, apple_tag> apple_weights(apple_range, 9.81);
vector_class<int, pear_tag> pear_counts(pear_range, 1);
vector_class<int, apple_tag> apple_counts(apple_range, 4);

Korektní použití

double total = 0.0;
for (auto i : pear_range)
  total += pear_counts[i] * pear_weights[i];
for (auto i : apple_range)
  total += apple_counts[i] * apple_weights[i];

Nesprávné použití

double total = 0.0;
for (auto i : pear_range)
  total += apple_counts[i] * apple_weights[i];  // COMPILE ERROR

Nápověda

vector_class<E,T> může být implementován pomocí std::vector<E> nebo std::unique_ptr<E[]>.

Koncept tag lze implementovat pomocí konceptu std::derived_from nebo konstanty std::is_base_of_v.

Požadovaný difference_type iterátoru lze najít pomocí std::make_signed_t.

std::numeric_limits<index_carrier_type<T>>::max() lze použít jako mez range_class<T>, ale nikdy nemůže být platným obsahem index_class<T>. Tuto hodnotu lze tedy použít k indikaci neplatné hodnoty produkované konstruktorem index_class<T> bez parametrů.

K bezpečné detekci, zda daný tag obsahuje typovou položku carrier_type, lze použít parciální specializaci pomocné šablony s vhodnou requires-expression.

operator* iterátoru spojeného s range_class<T> musí vracet hodnotou (a typová položka reference tudíž nemůže být reference), protože hodnoty vracené operátorem * nesmějí být nikde uloženy. Operátor * by teoreticky mohl vracet referenci na objekt uložený uvnitř iterátoru, tato implementace by však nefungovala správně v případech jako auto && x = *it++;.

EN:
Safe Vectors
Test Source Files

    test_01.cpp
    test_02.cpp
    test_03.cpp Split into 5 tests in Recodex
    test_util.hpp Used by all tests

Motivation

Strongly typed system of indexes, producing a compilation error whenever a wrong variable is used as an index to an array:

for (auto i : pears.range())
{
  std::cout << pears[i];  // CORRECT
  std::cout << apples[i]; // COMPILATION ERROR
}

This is achieved by attaching user-defined tags (as template arguments) to every container/range/index type:

vector_class<double,pear_tag> pears;
vector_class<double,apple_tag> apples;

The tags are user-defined structs, derived from tag_base to allow checking their correct usage by the concept tag.

As a safety feature, arithmetic operations on tagged indexes are not suported. This makes traditional (;i<n;i++) loops impossible. Instead, range-based for loops are supported, using objects representing ranges of properly tagged index values. Often, several containers will share the same range:

assert(a.range() == b.range()); // implies that a and b must have the same tag
for (auto i : a.range())
{
   c += a[i] * b[i];
}

In addition to safety, the system also provides the opportunity to reduce the size of index types for small containers, compared to the types std::size_t and std::ptrdiff_t used by standard containers and iterators. The underlying integer type may be specified as type element of a tag:

struct rare_tag : tag_base {
  using carrier_type = std::uint_least16_t;
}
std::vector<index_class<rare_tag>> indexes;  // 4-times smaller than with std::size_t

Elements to be implemented

The implementation shall be submitted into recodex as a header file named tagged_vector.hpp.
tag_base

The empty struct tag_base.
tag

The concept tag representing structs/classes derived from tag_base.
index_carrier_type

template<tag T> using index_carrier_type = /*...*/;

The type index_carrier_type<T> is the integral type used to implement index_class<T> and range_class<T>. The type shall be equal to T::carrier_type if it is defined. If T::carrier_type is not defined, index_carrier_type<T> shall be equal to std::size_t.
vector_class

template<typename E, tag T> class vector_class;

The class vector_class<E,T> represents an array of dynamically-defined size containing elements of the type E. The interface of vector_class<E,T> is essentially a subset of the interface of std::vector<E>, supporting the following operations (with slightly different arguments):

    default initialization producing an empty container with [0,0) range
    explicit constructor accepting two argument of the types range_class<T> and const E& to specify the allowed range of indexes and the initial value of all elements; the second argument shall have a default value of E()
    copy- and move- constructor and assignment (after a move operation, the source object shall be empty)
    element access using the [] operator accepting operands of type index_class<T> (the operator must return by reference and correctly propagate logical const-ness as usual with containers)
    the range() member function returning range_class<T>

Note: Compared to std::vector, vector_class does NOT support iterators, inserting/erasing elements (other than resizing), and a number of minor features like back(), data() etc. The range() method serves as a replacement for size().

Note: Unlike std::vector, vector_class supports indexing by arbitrary range (b,...,e-1), not only zero-based. However, these numbers can not be used directly as arguments to the [] operator; instead, the values must be encapsulated inside index_class<T>. Therefore, indexes are usually provided by a range-based for loop over the same range_class<T> object which was used to define the size of the container or over its copy obtained by calling range() on the container.
index_class

template<tag T> class index_class;

The class index_class<T> encapsulates a number of type index_carrier_type<T> serving as an index for accessing elements of vector_class<E,T> (for the same tag T but arbitrary element type E).

The main purpose of index_class<T> is to logically attach the tag T to numbers and to make index_class<T1> incompatible with vector_class<E,T2>::operator[] when T2 is different from T1.

index_class<T> shall support the following operations:

    default initialization, producing a value that will throw std::range_error("Uninitialized index") when used as argument to vector_class<E,T>::operator[] or any of the operations below (the only allowed action in such a state is the use as the target of assignment)
    copy-constructor and copy-assignment
    an explicit constructor accepting the type index_carrier_type<T> to initialize the internal number
    the member function value() returning the internal number as index_carrier_type<T>
    operators == and != (for two index_class objects with the same tag)
    formatted output to std::ostream using the << operator (equivalent to using << on the internal number)

range_class

template<tag T> class range_class;

The class range_class<T> shall support:

    a constructor accepting two arguments (ib, ie) of the type index_carrier_type<T>
    an explicit constructor accepting one argument (ie) of the type index_carrier_type<T>, equivalent to initializing with (0,ie)
    copy-constructor and copy-assignment
    operators == and != (for two range_class objects with the same tag)
    the member functions begin() and end() as in a std::ranges::range

A range_class object represents the interval [ib,ie) of integral numbers. The function begin and end shall return bidirectional iterators providing access to a virtual container containing elements of type index_class<T> with values {ib, ib+1, ib+2, ..., ie-1}. The value list must not be materialized, i.e. all the member functions of range_class<T> as well as all operations performed on the iterators returned from begin() and end() must work in constant time and constant memory.

Note: The presence of begin and end functions will allow the use of range_class<T> in range-based for-loops. However, range_class<T> does not have to satisfy the std::ranges::range concept.

Note: The purpose of range_class<T> is similar to std::ranges::iota_view except that the numbers in the sequence generated by range_class<T> are encapsulated in index_class<T>.
Iterators

The iterators returned from range_class<T>::begin() and range_class<T>::end() must support the requirements of bidirectional iterator over a virtual container, i.e. the following operations:

    the member type iterator_category equal to std::bidirectional_iterator_tag
    the member type value_type equal to index_class<T>
    the member type reference equal to index_class<T>
    the member type difference_type equal to the signed version of index_carrier_type<T>
    default initialization (producing an invalid iterator)
    copy-constructor and copy-assignment
    *it returning a value of type index_class<T>
    ++it, it++, --it, it--
    it1==it2, it1!=it2

Example
Tag structs

struct pear_tag : tag_base {};
struct apple_tag : tag_base {
  using carrier_type = std::ptrdiff_t;  // use signed type
};

Range types

range_class<pear_tag> pear_range(1, 10);
range_class<apple_tag> apple_range(-20, 20);

Containers

vector_class<double, pear_tag> pear_weights(pear_range, 3.14);
vector_class<double, apple_tag> apple_weights(apple_range, 9.81);
vector_class<int, pear_tag> pear_counts(pear_range, 1);
vector_class<int, apple_tag> apple_counts(apple_range, 4);

Correct usage

double total = 0.0;
for (auto i : pear_range)
  total += pear_counts[i] * pear_weights[i];
for (auto i : apple_range)
  total += apple_counts[i] * apple_weights[i];

Erroneous usage

double total = 0.0;
for (auto i : pear_range)
  total += apple_counts[i] * apple_weights[i];  // COMPILE ERROR

Hints

You may use std::vector<E> or std::unique_ptr<E[]> inside the implementation of vector_class<E,T>.

Consider the use of the std::derived_from concept or the std::is_base_of_v constant.

Use std::make_signed_t to determine difference_type in iterators.

std::numeric_limits<index_carrier_type<T>>::max() may appear as the bound of range_class<T> but can never be a valid contents of index_class<T>. Therefore, it may be used to indicate the default-initialized value of index_class<T>.

Use partial specialization with a suitable requires-expression to safely determine whether a tag class has a carrier_type member.

The operator* of the iterator class associated with the range_class<T> shall return by value (and the member type reference shall not be a reference), because the values returned by * are not stored anywhere. In theory, the * could return a reference to an object stored inside the iterator; however, it would not work correctly in cases like auto && x = *it++;.
