/** @file
  Interfejs do obsługi błędów

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_ERRORS_H
#define POLYNOMIALS_ERRORS_H

#include <stdio.h>
#include "stack.h"

/**
 * Wyświetla błąd o niepoprawnym poleceniu w wierszu.
 * @param[in] row : numer wiersza, w którym wystąpił błąd
 */
void ErrorWrongCommand(size_t row);

/**
 * Wyświetla błąd o wczytaniu niepoprawnego wielomianu w wierszu.
 * @param[in] row : numer wiersza, w którym wystąpił błąd
 */
void ErrorWrongPoly(size_t row);

/**
 * Wyświetla błąd o niepoprawnym argumencie polecenia DEG_BY.
 * @param[in] row : numer wiersza, w którym wystąpił błąd
 */
void ErrorDegBy(size_t row);

/**
 * Wyświetla błąd o niepoprawnym argumencie polecenia AT.
 * @param[in] row : numer wiersza, w którym wystąpił błąd
 */
void ErrorAt(size_t row);

/**
 * Wyświetla błąd dotyczący za małej liczby wielomianów na stosie do wykonania polecenia.
 * @param[in] row : numer wiersza, w którym wystąpił błąd
 */
void ErrorStackUnderflow(size_t row);

/**
 * Sprawdza czy na stosie jest co najmniej @p number_of_elements elementów.
 * @param[in] s : stos
 * @param[in] number_of_elements : liczba elementów
 * @param[in] row : numer wiersza, z którego zostało zczytane ostatnie polecenie
 * @return Czy na stosie jest co najmniej @p number_of_elements elementów?
 */
bool StackUnderflow(const Stack *s, size_t number_of_elements, size_t row);

#endif //POLYNOMIALS_ERRORS_H
