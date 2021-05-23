#ifndef POLYNOMIALS_ERRORS_H
#define POLYNOMIALS_ERRORS_H

#include <stdio.h>
#include "stack.h"

void ErrorWrongCommand(size_t row);
void ErrorWrongPoly(size_t row);
void ErrorDegBy(size_t row);
void ErrorAt(size_t row);
void ErrorStackUnderflow(size_t row);

bool StackUnderflow(const Stack *s, size_t number_of_elements, size_t row);

#endif //POLYNOMIALS_ERRORS_H
