#include <stdarg.h>
#include "calculator.h"

#define C PolyFromCoeff

static Mono M(Poly p, poly_exp_t n) {
    return MonoFromPoly(&p, n);
}

static Poly MakePolyHelper(poly_exp_t dummy, ...) {
      va_list list;
      va_start(list, dummy);
      size_t count = 0;
      while (true) {
            va_arg(list, Poly);
            if (va_arg(list, poly_exp_t) < 0)
                  break;
                count++;
      }
      va_start(list, dummy);
      Mono *arr = calloc(count, sizeof (Mono));
      CheckPtr(arr);
      for (size_t i = 0; i < count; ++i) {
            Poly p = va_arg(list, Poly);
            arr[i] = MonoFromPoly(&p, va_arg(list, poly_exp_t));
            assert(i == 0 || MonoGetExp(&arr[i]) > MonoGetExp(&arr[i - 1]));
          }
      va_end(list);
      Poly res = PolyAddMonos(count, arr);
      free(arr);
      return res;
    }

#define P(...) MakePolyHelper(0, __VA_ARGS__, PolyZero(), -1)

int main() {
    Stack s = InitStack();
    ParseInput(&s);
    StackClear(&s);
    return 0;
}