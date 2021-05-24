/** @file
  Interfejs kalulatora wielomianów

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALCULATOR_H
#define POLYNOMIALS_CALCULATOR_H

#include "errors.h"
#include "parser.h"

/**
 * Wypisuje wielomian z wierzchołku stosu.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Print(const Stack *s, size_t row);

/**
 * Wstawia na wierzchołek stosu wielomian tożsamościowo równy zeru.
 * @param[in] s : stos
 */
void Zero(Stack *s);

/**
 * Sprawdza, czy wielomian na wierzchołku stosu jest współczynnikiem – wypisuje na standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void IsCoeff(const Stack *s, size_t row);

/**
 * Wstawia na stos kopię wielomianu z wierzchołka.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Clone(Stack *s, size_t row);

/**
 * Sprawdza, czy wielomian na wierzchołku stosu jest tożsamościowo równy zeru -
 * wypisuje na standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void IsZero(const Stack *s, size_t row);

/**
 * Dodaje dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich sumę.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Add(Stack *s, size_t row);

/**
 * Odejmuje od wielomianu z wierzchołka wielomian pod wierzchołkiem, usuwa je i wstawia na wierzchołek stosu różnicę.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Sub(Stack *s, size_t row);

/**
 * Mnoży dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich iloczyn.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Mul(Stack *s, size_t row);

/**
 * Neguje wielomian na wierzchołku stosu.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Neg(Stack *s, size_t row);

/**
 * Sprawdza, czy dwa wielomiany na wierzchu stosu są równe – wypisuje na standardowe wyjście 0 lub 1.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void IsEq(const Stack *s, size_t row);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu (−1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Deg(const Stack *s, size_t row);

/**
 * Usuwa wielomian z wierzchołka stosu.
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void Pop(Stack *s, size_t row);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu ze względu na zmienną o numerze @p idx
 * (−1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] s : stos
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 * @param[in] idx : numer zmiennej
 */
void DegBy(Stack *s, size_t row, unsigned long long idx);

/**
 * Wylicza wartość wielomianu w punkcie @p x, usuwa wielomian z wierzchołka i wstawia na stos wynik operacji.
 * @param[in] s : stos
 * @param[in] x : wartość argumentu
 * @param[in] row : aktualny numer wiersza, z którego zostało wczytane polecenie
 */
void At(Stack *s, size_t row, long int x);

#endif //POLYNOMIALS_CALCULATOR_H
