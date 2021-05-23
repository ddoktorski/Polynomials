#include "errors.h"

void ErrorWrongCommand(size_t row) {
    fprintf(stderr, "ERROR %zu WRONG COMMAND\n", row);
}
void ErrorWrongPoly(size_t row) {
    fprintf(stderr, "ERROR %zu WRONG POLY\n", row);
}

void ErrorDegBy(size_t row) {
    fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", row);
}

void ErrorAt(size_t row) {
    fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", row);
}

void ErrorStackUnderflow(size_t row) {
    fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", row);
}

bool StackUnderflow(const Stack *s, size_t number_of_elements, size_t row) {
    if (StackGetSize(s) < number_of_elements) {
        ErrorStackUnderflow(row);
        return true;
    }
    return false;
}