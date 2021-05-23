#include "memory_handling.h"

void CheckPtr(const void *ptr) {
    if (ptr == NULL)
        exit(1);
}

size_t IncreaseSpace(size_t space) {
    return 2 * space;
}