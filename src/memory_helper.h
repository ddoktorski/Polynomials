/** @file
  Interfejs pomocnicznych funkcji do zarządzania pamięcią

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>, Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_MEMORY_HELPER_H
#define POLYNOMIALS_MEMORY_HELPER_H

/** Początkowy rozmiar nowo alokowanej tablicy. */
#define INIT_SIZE 10

#include "stdlib.h"

/**
 * Funkcja do sprawdzania, czy udało się zaalokować pamięć.
 * @param[in] ptr : wskaźnik
 */
void CheckPtr(const void *ptr);

/**
 * Zwraca powiększoną dwukrotnie wartość parametru @p space.
 * @param[in] space : liczba naturalna
 * @return dwukrotna wartość parametru @p
 */
size_t IncreaseSpace(size_t space);

#endif //POLYNOMIALS_MEMORY_HELPER_H
