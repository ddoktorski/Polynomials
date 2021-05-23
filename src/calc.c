#include <stdio.h>
#include "calc.h"
#include <stdlib.h>

void PrintHelper(const Poly *p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
        return;
    }

    for (size_t i = 0; i < p->size; ++i) {
        printf("(");
        PrintHelper(&p->arr[i].p);
        printf(",%d)", p->arr[i].exp);

        if (i != p->size - 1)
            printf("+");
    }
}

void Print(const Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row)) {
        PrintHelper(&s->polys[s->size - 1]);
        printf("\n");
    }
}

void Zero(Stack *s) {
    Poly p = PolyZero();
    StackPush(s, &p);
}

void IsCoeff(const Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row))
        printf("%d\n", PolyIsCoeff(&s->polys[s->size - 1]));
}

void Clone(Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row)) {
        Poly p = PolyClone(&s->polys[s->size - 1]);
        StackPush(s, &p);
    }
}

void IsZero(const Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row))
        printf("%d\n", PolyIsZero(&s->polys[s->size - 1]));
}

void Add(Stack *s, size_t row) {
    if (!StackUnderflow(s, 2, row)) {
        Poly p = PolyAdd(&s->polys[s->size - 1], &s->polys[s->size - 2]);
        StackPop(s);StackPop(s);
        StackPush(s, &p);
    }
}

void Sub(Stack *s, size_t row) {
    if (!StackUnderflow(s, 2, row)) {
        Poly p = PolySub(&s->polys[s->size - 1], &s->polys[s->size - 2]);
        StackPop(s);StackPop(s);
        StackPush(s, &p);
    }
}

void Mul(Stack *s, size_t row) {
    if (!StackUnderflow(s, 2, row)) {
        Poly p = PolyMul(&s->polys[StackGetSize(s) - 1], &s->polys[StackGetSize(s) - 2]);
        StackPop(s);StackPop(s);
        StackPush(s, &p);
    }
}

void Neg(Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row)) {
        Poly p = PolyNeg(&s->polys[StackGetSize(s) - 1]);
        StackPop(s);
        StackPush(s, &p);
    }
}

void IsEq(const Stack *s, size_t row) {
    if (!StackUnderflow(s, 2, row))
        printf("%d\n", PolyIsEq(&s->polys[StackGetSize(s) - 1], &s->polys[StackGetSize(s) - 2]));
}

void Deg(const Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row))
        printf("%d\n", PolyDeg(&s->polys[StackGetSize(s) - 1]));
}

void Pop(Stack *s, size_t row) {
    if (!StackUnderflow(s, 1, row))
        StackPop(s);
}

void DegBy(Stack *s, size_t row, unsigned long long idx) {
    if (!StackUnderflow(s, 1, row))
        printf("%d\n", PolyDegBy(&s->polys[StackGetSize(s) - 1], idx));
}

void At(Stack *s, size_t row, long int x) {
    if (!StackUnderflow(s, 1, row)) {
        Poly p = PolyAt(&s->polys[StackGetSize(s) - 1], x);
        StackPop(s);
        StackPush(s, &p);
    }
}

int main() {
    Stack s = InitStack();
    ParseInput(&s);
    StackClear(&s);
    return 0;
}
