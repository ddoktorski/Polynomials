#ifndef POLYNOMIALS_CALC_H
#define POLYNOMIALS_CALC_H

#include "errors.h"
#include "parser.h"
#include "stack.h"

void Print(const Stack *s, size_t row);
void Zero(Stack *s);
void IsCoeff(const Stack *s, size_t row);
void Clone(Stack *s, size_t row);
void IsZero(const Stack *s, size_t row);
void Add(Stack *s, size_t row);
void Sub(Stack *s, size_t row);
void Mul(Stack *s, size_t row);
void Neg(Stack *s, size_t row);
void IsEq(const Stack *s, size_t row);
void Deg(const Stack *s, size_t row);
void Pop(Stack *s, size_t row);
void DegBy(Stack *s, size_t row, unsigned long long idx);
void At(Stack *s, size_t row, long int x);

#endif //POLYNOMIALS_CALC_H
