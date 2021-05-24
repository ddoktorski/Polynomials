/** @file
  Implementacja pomocniczych funkcji do zarządzania pamięcią

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>, Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "memory_helper.h"

void CheckPtr(const void *ptr) {
    if (ptr == NULL)
        exit(1);
}

size_t IncreaseSpace(size_t space) {
    return 2 * space;
}