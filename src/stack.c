#include "stack.h"


static void CheckFreeSpace(Stack *s) {
    if (s->size == s->allocated_size) {
        s->allocated_size = IncreaseSpace(s->allocated_size);
        s->polys = (Poly*) realloc(s->polys, s->allocated_size * sizeof(Poly));
        //CheckPtr(s->polys);
    }
}

Stack InitStack() {
    Stack s;
    s.polys = (Poly*) malloc(INIT_SIZE * sizeof(Poly));
    CheckPtr(s.polys);
    s.size = 0;
    s.allocated_size = INIT_SIZE;
    return s;
}

size_t StackGetSize(const Stack *s) {
    return s->size;
}

bool IsEmpty(const Stack *s) {
    return StackGetSize(s) == 0;
}

void StackPush(Stack *s, const Poly *p) {
    CheckFreeSpace(s);
    s->polys[s->size++] = *p;
}

void StackPop(Stack *s) {
    if (!IsEmpty(s)) {
        PolyDestroy(&(s->polys[s->size - 1]));
        s->size--;
    }
}

void StackClear(Stack *s) {
    while (IsEmpty(s)) {
        StackPop(s);
    }
}