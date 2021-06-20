#include "calculator.h"

int main() {
    Stack s = InitStack();
    ParseInput(&s);
    StackClear(&s);
    return 0;
}