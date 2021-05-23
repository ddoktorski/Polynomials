#ifndef POLYNOMIALS_MEMORY_HANDLING_H
#define POLYNOMIALS_MEMORY_HANDLING_H

#define INIT_SIZE 10

#include "stddef.h"

void CheckPtr(const void *ptr);

size_t IncreaseSpace(size_t space);

#endif //POLYNOMIALS_MEMORY_HANDLING_H
