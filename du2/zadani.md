CZ:
Stručný popis

Vytvořte transparentní cache se smart pointry pro read-only data uložená na externím perzistentním úložišti.

Na externím úložišti (např. v souboru, db apod.) je uložena potenciálně rozsáhlá grafově orientovaná databáze. Každý uzel (vrchol) obsahuje data jednoho uživatelem definovaného typu (např. 32-bit integer nebo struktura) a pevný (uživatelsky definovaný) počet orientovaných hran (interních perzistentních odkazů) na jiné uzly v té samé datové kolekci (souboru, ...). Forma odkazu je implementačně definovaná úložištěm, může to být např. offset vhodné velikosti nebo jakýkoliv jiný typ. Úložiště poskytuje funkci pro načtení dat jednoho uzlu (včetně asociovaných hran) identifikovaného interním odkazem.

Klientský kód přistupuje k výše uvedeným datům přes chytré pointry, které transparentně zpřístupňují data načtená do cache. Sémantika odpovídá chování unique_ptr, tj. kopírovat chytré pointry je zakázáno, lze je move-ovat. Cache zajišťuje přítomnost načtených dat v paměti po celou dobu životnosti chytrého pointru referencujícího uzel. Chytré pointry mají metodu na získání dalších chytrých pointrů získaných z perzistentních odkazů referencovaného uzlu. Pro uvolňování cache je použita politika.

Vaším úkolem je implementovat cache, chytré pointry a konkrétní uvolňovací politiku. Rozhraní na externí úložiště, klientský (testovací) kód a další (testovací) politiky jsou implementovány Recodexem.
Příklad klientského fragmentu kódu

(detaily budou vysvětleny dále)

struct MyData {     // any self-contained type (i.e., without dynamic data and external references)
	uint32_t i32;
	std::array<char, 8> ca;
};

void f( const MyData& md) { .... }

cached_container< MyData, data_arity, EvictionOldestUnlock> cc { my_cache_size, external_storage_descriptor };
                                                       // data_arity denotes the number of pointers associated with each node
uint32_t sum = 0;
auto other_ptr = cc.null_ptr();                        // null (invalid) ptr
for( auto p = cc.root_ptr(); p; p = p.get_ptr(0)) {    // traversing from the root ptr while the following ptr is valid
    f( *p);                                            // MyData&
    other_ptr = p.get_ptr(1);                          // the referenced data are cached from now
    sum += p->i32 * other_ptr ? other_ptr->i32 : 1;
}

Detailní specifikace
Rozhraní na perzistentní úložiště

Toto rozhraní je implementováno v souboru file_access.h, který má být includován ve vašem řešení. Pro účely vývoje a vlastního lokálního ladění si vytvořte vlastní vhodnou implementaci tohoto rozhraní. Tu odevzdávat nebudete.

using file_descriptor = /* IMPLEMENTATION-DEFINED */;
using internal_ptr = /* IMPLEMENTATION-DEFINED */;                          // internal persistent ptr
constexpr internal_ptr null_internal_ptr = /* IMPLEMENTATION-DEFINED */;    // nullptr
constexpr internal_ptr root_internal_ptr = /* IMPLEMENTATION-DEFINED */;    // ptr to the root node

// data structure used by the FileAccess::read
template <typename ValueType, size_t arity>
struct persistent_node {
	ValueType value;                        // user-defined data
	std::array< internal_ptr, arity> ptr;   // array of interal ptrs
};

template <typename PersistentData>
class file_access {
public:
	file_access();
	bool read( file_descriptor fd, internal_ptr ip, PersistentData& item);   // reads the data (referenced by ip) incl. internal ptrs
private:
	/* IMPLEMENTATION-DEFINED */
};

Rozhraní na chytré pointry, cache a uvolňovací politiky

Následující rozhraní mají být vámi implementována v souboru pers_ptr.h, který bude includován v testovacích příkladech. Tento soubor odevzdáte jako řešení úlohy.

template < typename ValueType, /* IMPLEMENTATION-DEFINED */>
class pers_ptr {
public:
	pers_ptr( const pers_ptr&) = delete;
	pers_ptr( pers_ptr&&) noexcept;
   	~pers_ptr();
	pers_ptr& operator=( const pers_ptr&) = delete;
	pers_ptr& operator=( pers_ptr&&) noexcept;
	explicit operator bool() const noexcept;
	pers_ptr get_ptr( size_t index);
	const ValueType& operator*();
	const ValueType* operator->();
	/* other public methods may be IMPLEMENTATION-DEFINED */
private:
	/* IMPLEMENTATION-DEFINED */
};

pers_ptr je buď validní a referencuje data typu ValueType, která jsou umístěna v cache, nebo je nevalidní a nereferencuje nic. Validnost vrací operator bool() (stejně jako např. unique_ptr).

Move konstruktor přebírá pointr (včetně validnosti) od svého argumentu. Destruktor musí zajistit odemčení nacachovaných dat uzlu, tj. možnost jejich uvolnění z cache (kdy bude uvolněna určuje EvictionPolicy).

Přiřazení přebírá pointr (včetně validnosti) od svého argumentu. Pokud je před přiřazením cílový pointr validní, musí být jím referencovaný nacachovaný uzel odemčen. get_ptr vrací nově vytvořenou validní instanci pers_ptr na uzel referencovaný internal_ptr s indexem z parametru metody.

Operátory * a -> mají obvyklou sémantiku; vrací referenci resp. ukazatel na referencovaný uzel. Přítomnost v paměti a neměnnost lokace je zajištěna po dobu existence validního pers_ptr referencujícího tento uzel. Chování operátorů * a -> na nevalidním pers_ptr je nedefinované, tj. může dělat cokoliv.

template <typename ValueType, size_t arity, typename EvictionPolicy>
class cached_container {
public:
	cached_container( size_t, file_descriptor);
	pers_ptr<....> root_ptr();
	pers_ptr<....> null_ptr();
	/* other public methods may be IMPLEMENTATION-DEFINED */
private:
	/* IMPLEMENTATION-DEFINED */
};

Parametry konstruktoru jsou velikost cache (počet uzlů v paměti) a deskriptor otevřeného externího úložiště. Za získání deskriptoru je zodpovědný klientský kód, cache ho jen předá při volání file_access::read a nijak ho neinterpretuje.

root_ptr() vrátí validní pers_ptr na kořenový uzel (interně identifikovaný v externím úložišti root_internal_ptr). null_ptr() vrátí nevalidní pers_ptr.

class EvictionOldestUnlock {
public:
	using hint_type = /* IMPLEMENTATION-DEFINED */;
	void load( internal_ptr);                  // new cache node loaded from file
	hint_type unlock( internal_ptr);           // cached node unlocked (free to release)
	void relock( internal_ptr, hint_type);     // previously unlocked node from the cache that is locked again
	internal_ptr release();                    // eviction victim to be released
private:
	/* IMPLEMENTATION-DEFINED */
};

Rozhraní politiky je tvořeno čtyřmi metodami, které musí implementace cache volat při výskytu odpovídající události. Pro dosažení maximální možné efektivity si politika může nadefinovat vlastní typ hint_type, který může sloužit k rychlému nalezení vhodného záznamu. Pokud implementace politiky takový typ nepotřebuje, může si ho nadefinovat jako cokoliv jednoduchého (např. int) a v implementaci ho nepoužít.

load() je volán při načtení uzlu z externího úložiště.

unlock() je volán při ukončení platnosti validního pers_ptr, tj. při přiřazení nové hodnoty do proměnné a při zrušení proměnné.

relock() je volán při vytvoření pers_ptr na již nacachovaný uzel, tj. na uzel, na kterém byl volán unlock.

release() je volán při načítání uzlu z externího úložiště do cache v případě, že cache je zcela zaplněna. Funkce vrátí internal_ptr uzlu, který má být z cache uvolněn před načtením nového uzlu. Vlastní uvolnění implementuje cache.

Vaším úkolem je pomocí tohoto rozhraní implementovat politiku EvictionOldestUnlock, která ve funkci release vrátí nejstarší unlockovaný (a od té doby nerelockovaný) uzel.
Další upřesnění

Všechny operace musí být naimplementovány s maximální možnou efektivitou, musí mít ostře menší než lineární časovou složitost vzhledem k velikosti cache nebo počtu uzlů. Složitost operací by měla být konstantní (příp. amortizovaně konstantní, v průměru konstantní, apod.). Operátory * a -> musí přímo (tj. bez jakéhokoliv vyhledávání apod.) vrátit referenci resp. pointr na data. Věnujte proto velkou pozornost volbě datových struktur a efektivní implemenaci metod.

Všechny IMPLEMENTATION-DEFINED typy jsou copyable a assignable.

Nepoužívejte Recodex jako black-box debugger. Před odevzdáním řešení důkladně otestujte. Nemělo by být odevzdáno řešení, které neprojde kompilátorem.

Důsledně dodržujte Ethical Guidelines. V Recodexu je integrován prototyp antiplagiátorského modulu.

Pro případné dotazy na upřesnění zadání používejte Mattermost, nikoliv poznámky u zadání úlohy.
Soubory ke stažení

file_access.h

test.cpp

EN:
Brief description

Your task is to implement a transparent cache with smart pointers for read-only data stored on external persistent storage.

A potentially large graph-oriented database is stored on external storage (e.g. in a file, db, etc.). Each node (vertex) contains data of one user-defined type (e.g., 32-bit integer or structure) and fixed (user-defined) number of oriented edges (internal persistent references) to other nodes in the same data collection (file, ...). The type of the references is implementation-defined by the repository, it can be, for example, an offset or any other type. The repository provides a function to retrieve the data of a single node (including associated edges) identified by an internal reference.

The client code accesses the above data via smart pointers, which transparently make available the data loaded into the cache. The semantics is similar to unique_ptr, i.e., copying smart pointers is prohibited, they can be moved. The cache ensures the presence of the loaded data in memory for the entire lifetime of the smart pointer referencing the node. Smart pointers have a method to get additional smart pointers obtained from the referenced node's persistent links. A policy is used for the cache eviction.

Your task is to implement a cache, smart pointers, and a specific eviction policy. Interfaces to external storage, client (testing) code and other (testing) policies are implemented by Recodex.
Client code snippet

(details will be explained later)

struct MyData {     // any self-contained type (i.e., without dynamic data and external references)
	uint32_t i32;
	std::array<char, 8> ca;
};

void f( const MyData& md) { .... }

cached_container< MyData, data_arity, EvictionOldestUnlock> cc { my_cache_size, external_storage_descriptor };
                                                       // data_arity denotes the number of pointers associated with each node
uint32_t sum = 0;
auto other_ptr = cc.null_ptr();                        // null (invalid) ptr
for( auto p = cc.root_ptr(); p; p = p.get_ptr(0)) {    // traversing from the root ptr while the following ptr is valid
    f( *p);                                            // MyData&
    other_ptr = p.get_ptr(1);                          // the referenced data are cached from now
    sum += p->i32 * other_ptr ? other_ptr->i32 : 1;
}

Detailed specification
Interface of persistent storage

This interface is implemented in file_access.h to be included in your solution. For development purposes and your own local debugging, create your own suitable implementation of this interface. You will not submit this file.

using file_descriptor = /* IMPLEMENTATION-DEFINED */;
using internal_ptr = /* IMPLEMENTATION-DEFINED */;                          // internal persistent ptr
constexpr internal_ptr null_internal_ptr = /* IMPLEMENTATION-DEFINED */;    // nullptr
constexpr internal_ptr root_internal_ptr = /* IMPLEMENTATION-DEFINED */;    // ptr to the root node

// data structure used by the FileAccess::read
template <typename ValueType, size_t arity>
struct persistent_node {
	ValueType value;                        // user-defined data
	std::array< internal_ptr, arity> ptr;   // array of interal ptrs
};

template <typename PersistentData>
class file_access {
public:
	file_access();
	bool read( file_descriptor fd, internal_ptr ip, PersistentData& item);   // reads the data (referenced by ip) incl. internal ptrs
private:
	/* IMPLEMENTATION-DEFINED */
};

Interface of smart pointers, cache and release policies

The following interfaces should be implemented by you in the file pers_ptr.h, which will be included in the test examples. You should submit this file as a solution to the assignment.

template < typename ValueType, /* IMPLEMENTATION-DEFINED */>
class pers_ptr {
public:
	pers_ptr( const pers_ptr&) = delete;
	pers_ptr( pers_ptr&&) noexcept;
   	~pers_ptr();
	pers_ptr& operator=( const pers_ptr&) = delete;
	pers_ptr& operator=( pers_ptr&&) noexcept;
	explicit operator bool() const noexcept;
	pers_ptr get_ptr( size_t index);
	const ValueType& operator*();
	const ValueType* operator->();
	/* other public methods may be IMPLEMENTATION-DEFINED */
private:
	/* IMPLEMENTATION-DEFINED */
};

pers_ptr is either valid and references data of type ValueType that is cached, or invalid and references nothing. Validity is returned by the bool() operator (similarly as unique_ptr).

The move constructor takes a pointer (including validity) from its argument. The destructor must ensure the unlocking of the cached data of the node, i.e. the possibility of releasing it from the cache (EvictionPolicy determines when it will be released).

The move assignment takes a pointer (including validity) from its argument. If the target pointer is valid before assignment, the cached node referenced by it must be unlocked. get_ptr returns a newly created valid instance of pers_ptr to the node referenced by internal_ptr with the index from the method parameter.

The * and -> operators have the usual semantics; they return a reference or a pointer to the referenced node. The presence in memory and immutability of the location is ensured for the entire lifetime of a valid pers_ptr referencing this node. The behavior of the * and -> operators on an invalid pers_ptr is undefined, i.e. it can do anything.

template <typename ValueType, size_t arity, typename EvictionPolicy>
class cached_container {
public:
	cached_container( size_t, file_descriptor);
	pers_ptr<....> root_ptr();
	pers_ptr<....> null_ptr();
	/* other public methods may be IMPLEMENTATION-DEFINED */
private:
	/* IMPLEMENTATION-DEFINED */
};

The parameters of the constructor are the size of the cache (the number of nodes in memory) and the descriptor of the open external storage. The client code is responsible for obtaining the descriptor, the cache just passes it when calling file_access::read and does not interpret it in any way.

root_ptr() returns a valid pers_ptr to the root node (identified internally by root_internal_ptr in external storage). null_ptr() returns an invalid pers_ptr.

class EvictionOldestUnlock {
public:
	using hint_type = /* IMPLEMENTATION-DEFINED */;
	void load( internal_ptr);                  // new cache node loaded from file
	hint_type unlock( internal_ptr);           // cached node unlocked (free to release)
	void relock( internal_ptr, hint_type);     // previously unlocked node from the cache that is locked again
	internal_ptr release();                    // eviction victim to be released
private:
	/* IMPLEMENTATION-DEFINED */
};

The policy interface consists of four methods that the cache implementation must call when the corresponding event occurs. To achieve the maximum possible efficiency, the policy can define its own type hint_type, which can be used to find a suitable record. If the policy implementation does not need such a type, it can define it as anything simple (e.g., int) and not use it in the implementation.

load() is called when a node is loaded from external storage.

unlock() is called when a valid pers_ptr expires, i.e. when a new value is assigned to the variable or when the variable is destroyed.

relock() is called when creating pers_ptr on an already cached node, i.e. on the node on which unlock was called.

release() is called when loading a node from external storage into the cache in case the cache is completely full. The function returns the internal_ptr of the node to be released from the cache before the new node is loaded. The release code is implemented by the cache.

Using this interface, your task is to implement the EvictionOldestUnlock policy, which returns the oldest unlocked (and not relocked since then) node in the release function.
More details

All operations must be implemented with the maximum possible efficiency, they must have sublinear time complexity with respect to the size of the cache or the number of nodes. The complexity of the operations should be constant (or amortized constant, constant on average, etc.). The operators * and -> must directly (i.e. without any lookup, etc.) return a reference or pointer to data. Therefore, pay close attention to the choice of data structures and efficient implementation of the interfaces.

All IMPLEMENTATION-DEFINED types are copyable and assignable.

Do not use Recodex as a black-box debugger. Test your solution thoroughly before submitting it. A solution that does not pass the compiler should not be submitted.

Strictly follow the Ethical Guidelines. A prototype of anti-plagiarism module is integrated into Recodex.

For any questions use Mattermost, not the notes in the assignment.
Downloadable files

file_access.h

test.cpp
