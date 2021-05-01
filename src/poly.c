#include "poly.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int min(int x, int y) {
    return (x < y) ? x : y;
}

int max(int x, int y) {
    return (x > y) ? x : y;
}

poly_coeff_t power(poly_coeff_t x, poly_exp_t exp) {
    if (exp == 0)
        return 1;
    return x * power(x, exp - 1);
}

Poly PolyAlloc(size_t size) {
    Poly p;
    p.size = size;
    p.arr = (Mono*)calloc(size, sizeof(Mono));
    CHECK_PTR(p.arr);
    return p;
}

void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < PolyGetSize(p); ++i)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
        p->arr = NULL;
        p->coeff = 0;
    }
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p))
        return *p;

    Poly copy = PolyAlloc(PolyGetSize(p));
    for (size_t i = 0; i < PolyGetSize(&copy); ++i)
        copy.arr[i] = MonoClone(&p->arr[i]);

    return copy;
}

bool MonoIsCoeff(const Mono *m) {
    if (m->exp == 0 && PolyIsCoeff(&m->p))
        return true;
    else if (m->exp != 0 || (!PolyIsCoeff(&m->p) && m->p.size > 1))
        return false;
    else
        return MonoIsCoeff(&m->p.arr[0]);
}

int CompareMonosByExp(const void *a, const void *b) {
    return ((const Mono*)a)->exp - ((const Mono*)b)->exp;
}

Poly PolyAddPolyCoeff(const Poly *p, poly_coeff_t q_coeff) {
    if (q_coeff == 0)
        return PolyClone(p);

    if (PolyIsCoeff(p))
        return PolyCoeffAddPolyCoeff(p->coeff, q_coeff);
    else if (MonoGetExp(&p->arr[0]) == 0) {
        Poly added = PolyAddPolyCoeff(&p->arr[0].p, q_coeff);
        if (PolyIsZero(&added)) {
            PolyDestroy(&added);
            Poly new_poly = PolyAlloc(PolyGetSize(p) - 1);
            for (size_t i = 0; i < PolyGetSize(&new_poly); ++i)
                new_poly.arr[i] = MonoClone(&p->arr[i + 1]);
            return new_poly;
        }
        else if (PolyIsCoeff(&added) && PolyGetSize(p) == 1)
            return added;
        else {
            Poly new_poly = PolyAlloc(PolyGetSize(p));
            new_poly.arr[0].p = added;
            for (size_t i = 1; i < PolyGetSize(&new_poly); ++i)
                new_poly.arr[i] = MonoClone(&p->arr[i]);
            return new_poly;
        }
    }
    else {
        Poly q = PolyFromCoeff(q_coeff);
        Mono new_mono = MonoFromPoly(&q, 0);
        Poly new_poly = PolyAlloc(PolyGetSize(p) + 1);
        new_poly.arr[0] = new_mono;
        for (size_t i = 0; i < PolyGetSize(p); ++i)
            new_poly.arr[i + 1] = MonoClone(&p->arr[i]);
        return new_poly;
    }
}

Poly PolyAddPoly(const Poly *p, const Poly *q) {
    size_t i = 0, j = 0;
    Poly added = PolyAlloc(PolyGetSize(p) + PolyGetSize(q));
    size_t real_size = 0;
    while (i < PolyGetSize(p) && j < PolyGetSize(q)) {
        if (MonoGetExp(&p->arr[i]) < MonoGetExp(&q->arr[j])) {
            added.arr[real_size] = MonoClone(&p->arr[i]);
            real_size++;
            i++;
        }
        else if (MonoGetExp(&p->arr[i]) > MonoGetExp(&q->arr[j])) {
            added.arr[real_size] = MonoClone(&q->arr[j]);
            real_size++;
            j++;
        }
        else {
            Poly add_same_exp = PolyAdd(&p->arr[i].p, &q->arr[j].p);
            if (!(PolyIsZero(&add_same_exp))) {
                Mono m = MonoFromPoly(&add_same_exp, MonoGetExp(&p->arr[i]));
                added.arr[real_size++] = m;
            }
            else
                PolyDestroy(&add_same_exp);
            i++; j++;
        }
    }
    while (i < PolyGetSize(p)) {
        added.arr[real_size] = MonoClone(&p->arr[i]);
        real_size++;
        i++;
    }
    while (j < PolyGetSize(q)) {
        added.arr[real_size] = MonoClone(&q->arr[j]);
        real_size++;
        j++;
    }

    if (real_size == 0) {
        free(added.arr);
        added.arr = NULL;
        added.coeff = 0;
    }
    else if (real_size == 1 && MonoGetExp(&added.arr[0]) == 0 && PolyIsCoeff(&added.arr[0].p)) {
        added.coeff = added.arr[0].p.coeff;
        free(added.arr);
        added.arr = NULL;
    }
    else {
        added.size = real_size;
        added.arr = (Mono*)realloc(added.arr, real_size * sizeof(Mono));
        CHECK_PTR(added.arr);
    }
    return added;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(q))
        return PolyAddPolyCoeff(p, q->coeff);
    else if (PolyIsCoeff(p))
        return PolyAddPolyCoeff(q, p->coeff);
    else
        return PolyAddPoly(p, q);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyZero();

    Mono *sorted_monos = (Mono*)calloc(count, sizeof(Mono));
    CHECK_PTR(sorted_monos);
    memcpy(sorted_monos, monos, count * sizeof(Mono));
    qsort(sorted_monos, count, sizeof(Mono), CompareMonosByExp);

    size_t real_size = 0;

    for (size_t i = 0; i < count; ++i) {
        if (i == 0 || MonoGetExp(&sorted_monos[i]) != MonoGetExp(&sorted_monos[real_size - 1]))
            sorted_monos[real_size++] = sorted_monos[i];
        else {
            poly_exp_t exp = MonoGetExp(&sorted_monos[i]);
            Poly sum = PolyAdd(&sorted_monos[real_size - 1].p, &sorted_monos[i].p);
            MonoDestroy(&sorted_monos[i]);
            MonoDestroy(&sorted_monos[real_size - 1]);

            if (PolyIsZero(&sum)) {
                real_size = (real_size == 0) ? 0 : real_size - 1;
                PolyDestroy(&sum);
            }
            else
                sorted_monos[real_size - 1] = MonoFromPoly(&sum, exp);
        }
    }

    if (real_size == 0)
        return PolyZero();
    else if (real_size == 1 && MonoIsCoeff(&sorted_monos[0])) {
        Poly *pom = &sorted_monos[0].p;
        while (pom->arr != NULL) {
            pom = &pom->arr[0].p;
        }
        poly_coeff_t coeff = pom->coeff;
        MonoDestroy(&sorted_monos[0]);
        return PolyFromCoeff(coeff);
    }

    sorted_monos = (Mono*)realloc(sorted_monos, real_size * sizeof(Mono));
    CHECK_PTR(sorted_monos);
    return (Poly) {.arr = sorted_monos, .size = real_size};
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) return p->coeff == q->coeff;
    if (PolyIsCoeff(q) || PolyIsCoeff(p)) return false;
    if (p->size != q->size) return false;
    bool eq = true;
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        if (MonoGetExp(&p->arr[i]) != MonoGetExp(&q->arr[i]))
            return false;
        eq &= PolyIsEq(&p->arr[i].p, &q->arr[i].p);
    }
    return eq;
}

poly_exp_t PolyDegHelper(const Poly *p, poly_exp_t deg_so_far) {
    if (PolyIsCoeff(p))
        return deg_so_far;
    poly_exp_t deg = 0;
    for (size_t i = 0; i < PolyGetSize(p); ++i)
        deg = max(deg, PolyDegHelper(&p->arr[i].p, deg_so_far + MonoGetExp(&p->arr[i])));
    return deg;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    return PolyDegHelper(p, 0);
}

poly_exp_t PolyDegByHelper(const Poly *p, size_t var_idx, size_t current_index) {
    if (PolyIsCoeff(p))
        return 0;
    poly_exp_t deg = 0;
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        if (current_index < var_idx)
            deg = max(deg, PolyDegByHelper(&p->arr[i].p, var_idx, current_index + 1));
        else if (current_index == var_idx)
            deg = max(deg, MonoGetExp(&p->arr[i]));
    }
    return deg;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p))
        return -1;
    return PolyDegByHelper(p, var_idx, 0);
}

void PolyNegHelper(Poly *clone) {
    if (PolyIsCoeff(clone)) {
        clone->coeff *= (-1);
        return;
    }

    for (size_t i = 0; i < PolyGetSize(clone); ++i)
        PolyNegHelper(&clone->arr[i].p);
}

Poly PolyNeg(const Poly *p) {
    Poly clone = PolyClone(p);
    PolyNegHelper(&clone);
    return clone;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly neg_q = PolyNeg(q);
    Poly result = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return result;
}

Poly PolyMulPolyCoeff(const Poly *p, poly_coeff_t q_coeff) {
    if (q_coeff == 0)
        return PolyZero();
    else if (q_coeff == 1)
        return *p;
    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff * q_coeff);

    Poly new_poly = PolyAlloc(PolyGetSize(p));
    size_t real_size = 0;
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        Poly multiplied = PolyMulPolyCoeff(&p->arr[i].p, q_coeff);
        if (!PolyIsZero(&multiplied))
            new_poly.arr[real_size++] = MonoFromPoly(&multiplied, MonoGetExp(&p->arr[i]));
    }

    if (real_size == 0) {
        PolyDestroy(&new_poly);
        return PolyZero();
    }
    new_poly.arr = (Mono*)realloc(new_poly.arr, real_size * sizeof(Mono));
    new_poly.size = real_size;
    return new_poly;
}

Poly PolyMulPoly(const Poly *p, const Poly *q) {
    Mono *monos = (Mono*)calloc(PolyGetSize(p) * PolyGetSize(q), sizeof(Mono));
    size_t real_size = 0;
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        for (size_t j = 0; j < PolyGetSize(q); ++j) {
            Poly multiplied = PolyMul(&p->arr[i].p, &q->arr[j].p);
            if (!PolyIsZero(&multiplied)) {
                Mono m = MonoFromPoly(&multiplied, MonoGetExp(&p->arr[i]) + MonoGetExp(&q->arr[j]));
                monos[real_size++] = m;
            }
        }
    }
    if (real_size == 0) {
        free(monos);
        return PolyZero();
    }
    monos = (Mono*)realloc(monos, real_size * sizeof(Mono));
    Poly result = PolyAddMonos(real_size, monos);
    free(monos); // mozliwe memory leak
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p))
        return PolyMulPolyCoeff(q, p->coeff);
    else if (PolyIsCoeff(q))
        return PolyMulPolyCoeff(p, q->coeff);
    else
        return PolyMulPoly(p, q);
}


Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p))
        return *p;

    Poly result = PolyZero();
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        poly_coeff_t value = power(x, MonoGetExp(&p->arr[i]));
        Poly multiply = PolyMulPolyCoeff(&p->arr[i].p, value);
        Poly add = PolyAdd(&multiply, &result);
        //PolyDestroy(&multiply); // double free detected
        PolyDestroy(&result);
        result = add;
    }
    return result;
}

void PolyPrintHelper(const Poly *p, int i) {
    if (PolyIsZero(p))
        printf("ZERO");
    else if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
    }
    else {
        for (size_t j = 0; j < p->size; ++j) {
            printf("x_%d^%d(", i, p->arr[j].exp);
            PolyPrintHelper(&p->arr[j].p, i + 1);
            (j == p->size - 1) ? printf(")") : printf(") + ");
        }
    }
}

void PolyPrint(const Poly *p) {
    PolyPrintHelper(p, 0);
    printf("\n");
}