/** @file
  Interfejs dla parsera wielomianów

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_PARSER_H
#define POLYNOMIALS_PARSER_H

#include "stack.h"
#include "calculator.h"
#include <string.h>

/** Typ reprezentujący unsigned long long, dla skrócenia kodu. */
typedef unsigned long long ull;

/** Struktura do przechowywania napisu. */
typedef struct String{
    char *arr; ///< tablica przechowująca napis
    size_t size; ///< rozmiar napisu
    size_t allocated_size; ///< rozmiar zaalokowanej pamięci w tablicy arr
} String;

/** Struktura do przechowywania liczby całkowitej z zakresu od -ULLONG_MAX do ULLONG_MAX. */
typedef struct Number {
    bool minus; ///< true jeżeli liczba jest ujemna
    ull value; ///< wartość bezwzględna liczby
} Number;

/** Struktura przechowująca tablicę jednomianów. */
typedef struct MonosArr {
    Mono *arr; ///< tablica jednomianów
    size_t size; ///< liczba jednomianów w tablicy
    size_t allocated_size; ///< rozmiar zaalokowanej pamięci w tablicy arr
} MonosArr;

/** Struktura pomocnicza do przechowywania niektórych informacji o stanie wczytywanego wiersza. */
typedef struct ParserProtector {
    bool error; ///< czy wystąpił błąd
    bool end_of_file; ///< czy został osiągnięty koniec pliku
    bool end_of_line; ///< czy został osiągnięty koniec wiersza
} ParserProtector;

/**
 * Tworzy pusty napis.
 * @return pusty napis
 */
String CreateString();

/**
 * Sprawdza, czy napis @p str posiada zaalokowaną pamięć na dodanie nowego znaku, jeżeli nie
 * to alokuje dodatkową pamięć.
 * @param[in] str : napis
 */
void CheckStringSpace(String *str);

/**
 * Usuwa napis z pamięci.
 * @param[in] s : napis do usunięcia
 */
void DestroyString(String *s);

/**
 * Tworzy nowy obiekt struktuyr MonosArr.
 * @return obiekt struktury MonosArr przechowujący pustą tablicę jednomianów
 */
MonosArr CreateMonosArr();

/**
 * Sprawdza, czy jest wystarczająca ilość pamięci zaalokowana w @p monos, aby dodać
 * kolejny jednomian. Jeżeli nie ma to alokuje dodatkową pamięć.
 * @param[in] monos : tablica jednomianów
 */
void CheckMonosArrSpace(MonosArr *monos);

/**
 * Usuwa tablicę jednomianów z pamięci (nie zwalnia pamięci jednomianów).
 * @param[in] monos : tablica jednomianów
 */
void DestroyMonosArr(MonosArr *monos);

/**
 * Parsuje liczbę.
 * @param[in,out] error : informacja o tym czy wystąpił błąd
 * @return wczytana liczba
 */
Number ParseNumber(bool *error);

/**
 * Parsuje jednomian.
 * @param[in,out] protector : informacje o stanie wczytywanego wiersza
 * @return wczytany jednomian
 */
Mono ParseMono(ParserProtector *protector);

/**
 * Parsuje wielomian.
 * @param[in,out] protector : informacje o stanie wczytywanego wiersza
 * @return wczytany wielomian
 */
Poly ParsePoly(ParserProtector *protector);

/**
 * Wczytuje polecenie, jego wartość jest zapisywana do zmiennej @p command.
 * @param[in] command : napis do którego będzie wczytane polecenie
 */
void ParseCommand(String *command);


/**
 * Wykonuje polecenie kalkulatora. Jeżeli polecenie jest nieprawidłowe lub nieprawidłowy jest
 * argument polecenia DEG_BY lub AT to wówczas wypisuje odpowiedni błąd.
 * @param[in,out] s : stos
 * @param[in] command : polecenie
 * @param[in,out] protector : informacje o stanie wczytywanego wiersza
 * @param[in] row : numer wiersza, w którym zostało wczytane polecenie
 */
void ExecuteCommand(Stack *s, String *command, ParserProtector *protector, size_t row);

/**
 * Przeprowadza operacje wczytywania wielomianów oraz poleceń.
 * @param[in] s : stos
 */
void ParseInput(Stack *s);

#endif //POLYNOMIALS_PARSER_H
