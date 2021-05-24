/** @file
  Interfejs do obsługi stosu

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_STACK_H
#define POLYNOMIALS_STACK_H

#include "poly.h"
#include "memory_helper.h"

/** Struktura przechowująca stos wielomianów. */
typedef struct Stack {
    Poly *polys; ///< wielomiany przechowywane na stosie
    size_t size; ///< rozmiar stosu
    size_t allocated_size; ///< rozmiar zaalakowanej pamięci w tablicy polys
} Stack;


/** Tworzy pusty stos. */
Stack InitStack();

/**
 * Daje liczbę elementów na stosie.
 * @param[in] s : stos
 * @return liczba elementów przechowywanych na stosie
 */
size_t StackGetSize(const Stack *s);

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] s : stos
 * @return Czy stos jest pusty?
 */
bool IsEmpty(const Stack *s);

/**
 * Dodaje wielomian na stos.
 * @param[in] s : stos
 * @param[in] p : wielomian, który będzie dodany na stos
 */
void StackPush(Stack *s, const Poly *p);

/**
 * Usuwa wielomian z góry stosu.
 * @param[in] s : stos
 */
void StackPop(Stack *s);

/**
 * Usuwa wszystkie elementy ze stosu.
 * @param[in] s : stos
 */
void StackClear(Stack *s);


#endif //POLYNOMIALS_STACK_H
