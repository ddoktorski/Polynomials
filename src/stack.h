#ifndef POLYNOMIALS_STACK_H
#define POLYNOMIALS_STACK_H

#include <stdlib.h>
#include "poly.h"
#include "memory_handling.h"

typedef struct Stack {
    Poly *polys;
    size_t size;
    size_t allocated_size;
} Stack;

Stack InitStack();
size_t StackGetSize(const Stack *s);
bool IsEmpty(const Stack *s);
void StackPush(Stack *s, const Poly *p);
void StackPop(Stack *s);
void StackClear(Stack *s);


#endif //POLYNOMIALS_STACK_H
