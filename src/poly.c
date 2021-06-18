/** @file
  Implementacja funkcji do obsługi wielomianów rzadkich wielu zmiennych

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly.h"
#include <string.h>

/**
 * Funkcja pomocnicza do obliczania większej z dwóch liczb
 * @param x : pierwsza liczba
 * @param y : druga liczba
 * @return większą z liczb @f$x@f$, @f$y@f$
 */
static int max(int x, int y) {
    return (x > y) ? x : y;
}

/**
 * Dodaje dwa wielomiany stałe
 * @param[in] p_coeff : współczynnik w wielomianie stałym @f$p@f$
 * @param[in] q_coeff : współczynnik w wielomianie stałym @f$q@f$
 * @return @f$p + q@f$
 */
static inline Poly PolyCoeffAddPolyCoeff(poly_coeff_t p_coeff, poly_coeff_t q_coeff) {
    return PolyFromCoeff(p_coeff + q_coeff);
}

/**
 * Dodaje dwa wielomiany, z czego wielomian @f$q@f$ jest wielomianem stałym
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q_coeff : współczynnik w wielomianie stałym @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddPolyCoeff(const Poly *p, poly_coeff_t q_coeff);

/**
 * Dodaje dwa wielomiany, które nie są wielomianami stałymi
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddPoly(const Poly *p, const Poly *q);

/**
 * Mnoży wielomian przez współczynnik.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q_coeff : współczynnik w wielomianie stałym @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulPolyCoeff(const Poly *p, poly_coeff_t q_coeff);

/**
 * Mnoży dwa wielomiany, które nie są wielomianami stałymi.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulPoly(const Poly *p, const Poly *q);

/** Funkcja pomocnicza do funkcji Power, jest to implementacja algorytmu szybkiego potęgowania */
static poly_coeff_t PowerHelper(poly_coeff_t y, poly_coeff_t x, poly_exp_t n) {
    if (n == 0)
        return y;
    else if (n % 2 == 0)
        return PowerHelper(y, x * x, n / 2);
    else // n odd
        return PowerHelper(x * y, x * x, n / 2);
}

/**
 * Oblicza @f$x^n@f$ w optymalny sposób.
 * @param[in] x - podstawa
 * @param[in] n - wykładnik
 * @return @f$x^n@f$
 */
static inline poly_coeff_t Power(poly_coeff_t x, poly_exp_t n) {
    assert(n >= 0);
    return PowerHelper(1, x, n);
}

static Poly PolyPowerHelper(Poly *q, Poly *p, poly_exp_t n) {
    if (n == 0) {
        return *q;
    }
    else {
        Poly square = PolyMul(p, p);
        if (n % 2 == 0) {
            PolyDestroy(p);
            Poly res = PolyPowerHelper(q, &square, n / 2);
            PolyDestroy(&square);
            return res;
        }
        else {
            Poly mul = PolyMul(p, q);
            PolyDestroy(p);
            PolyDestroy(q);
            Poly res = PolyPowerHelper(&mul, &square, n / 2);
            PolyDestroy(&square);
            return res;
        }
    }
}

Poly PolyPower(const Poly *p, poly_exp_t n) {
    assert(n >= 0);
    Poly one = PolyFromCoeff(1);
    Poly copy = PolyClone(p);
    Poly res = PolyPowerHelper(&one, &copy, n);
    PolyDestroy(&copy);
    return res;
}



Poly PolyAlloc(size_t size) {
    Poly p;
    p.size = size;
    p.arr = (Mono*)calloc(size, sizeof(Mono));
    CheckPtr(p.arr);
    return p;
}

/**
 * Realokuje pamięć w wielomianie @f$p@f$
 * @param[in, out] p : wielomian
 * @param[in] size : nowa długość tablicy jednomianów
 */
void PolyRealloc(Poly *p, size_t size) {
    if (size == 0)
        PolyDestroy(p);
    else {
        p->size = size;
        p->arr = (Mono* )realloc(p->arr, size * sizeof(Mono));
        CheckPtr(p->arr);
    }
}

void PolyDestroy(Poly *p) {
    assert(p != NULL);
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < PolyGetSize(p); ++i)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
        p->arr = NULL;
        p->coeff = 0;
    }
}

Poly PolyClone(const Poly *p) {
    assert(p != NULL);
    if (PolyIsCoeff(p))
        return *p;

    Poly copy = PolyAlloc(PolyGetSize(p));
    for (size_t i = 0; i < PolyGetSize(&copy); ++i)
        copy.arr[i] = MonoClone(&p->arr[i]);

    return copy;
}

bool MonoIsCoeff(const Mono *m, poly_coeff_t *coeff) {
    assert(m != NULL);
    if (m->exp == 0 && PolyIsCoeff(&m->p)) {
        *coeff = m->p.coeff;
        return true;
    }
    else if (m->exp != 0 || (!PolyIsCoeff(&m->p) && m->p.size > 1))
        return false;
    else
        return MonoIsCoeff(&m->p.arr[0], coeff);
}

/**
 * Pomocnicza funkcja do sortowania jednomianow
 * @param a : wskaźnik do pierwszego jednomianu
 * @param b : wskaźnik do drugiego jednomianu
 * @return różnica wykładników jednomianów
 */
static int CompareMonosByExp(const void *a, const void *b) {
    return ((const Mono*)a)->exp - ((const Mono*)b)->exp;
}

static Poly PolyAddPolyCoeff(const Poly *p, poly_coeff_t q_coeff) {
    assert(p != NULL);
    if (q_coeff == 0)
        return PolyClone(p);

    if (PolyIsCoeff(p))
        return PolyCoeffAddPolyCoeff(p->coeff, q_coeff);
    else if (MonoGetExp(&p->arr[0]) == 0) {
        Poly added = PolyAddPolyCoeff(&p->arr[0].p, q_coeff);
        // jezeli po dodaniu otrzymujemy wielomian zerowy to zmniejszmay tablice jednomianow o 1 i przesuwamy pozostale jednomiany
        if (PolyIsZero(&added)) {
            Poly new_poly = PolyAlloc(PolyGetSize(p) - 1);
            for (size_t i = 0; i < PolyGetSize(&new_poly); ++i)
                new_poly.arr[i] = MonoClone(&p->arr[i + 1]);
            return new_poly;
        }
        // jezeli added jest wielomianem stalym i p sklada sie tylko z jednego jednomianu
        // to zwracamy added
        else if (PolyIsCoeff(&added) && PolyGetSize(p) == 1)
            return added;
        // wpp aktualizujemy jednomian pod indeksem 0 w wielomianie p
        else {
            Poly new_poly = PolyAlloc(PolyGetSize(p));
            new_poly.arr[0].p = added;
            for (size_t i = 1; i < PolyGetSize(&new_poly); ++i)
                new_poly.arr[i] = MonoClone(&p->arr[i]);
            return new_poly;
        }
    }
    // jezeli potega pierwszego jednomianu w p jest wieksza od 0
    // to przesuwamy wszyskie jednomiany o 1 a pod indeksem 0 wstawiamy jednomian, ktory ma
    // wielomian staly rowny q_coeff
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

static Poly PolyAddPoly(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    size_t i = 0, j = 0;
    Poly added = PolyAlloc(PolyGetSize(p) + PolyGetSize(q));
    size_t real_size = 0;

    while (i < PolyGetSize(p) && j < PolyGetSize(q)) {
        if (MonoGetExp(&p->arr[i]) < MonoGetExp(&q->arr[j]))
            added.arr[real_size++] = MonoClone(&p->arr[i++]);
        else if (MonoGetExp(&p->arr[i]) > MonoGetExp(&q->arr[j]))
            added.arr[real_size++] = MonoClone(&q->arr[j++]);
        // jezeli wykladniki sa rowne to musimy dodac wielomiany
        else {
            Poly add_same_exp = PolyAdd(&p->arr[i].p, &q->arr[j].p);
            // jezeli wielomian po zsumowaniu jest niezerowy to go dodajemy
            if (!(PolyIsZero(&add_same_exp))) {
                Mono m = MonoFromPoly(&add_same_exp, MonoGetExp(&p->arr[i]));
                added.arr[real_size++] = m;
            }
            i++; j++;
        }
    }
    // po tym jak przejdziemy jeden caly wielomian to przepisujemy pozostale wartosci z drugiego wielomianu
    // co najmniej jedna z ponizszych petli się nie wykona
    while (i < PolyGetSize(p)) {
        added.arr[real_size++] = MonoClone(&p->arr[i++]);
    }
    while (j < PolyGetSize(q)) {
        added.arr[real_size++] = MonoClone(&q->arr[j++]);
    }

    // sprawdzamy czy utworzony wielomian nie jest tak naprawde jednomianem
    poly_coeff_t coeff;
    if (real_size == 1 && MonoIsCoeff(&added.arr[0], &coeff)) {
        PolyDestroy(&added);
        return PolyFromCoeff(coeff);
    }

    PolyRealloc(&added, real_size);
    return added;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    if (PolyIsCoeff(q))
        return PolyAddPolyCoeff(p, q->coeff);
    else if (PolyIsCoeff(p))
        return PolyAddPolyCoeff(q, p->coeff);
    else
        return PolyAddPoly(p, q);
}

Poly PolyAddMonosHelper(size_t count, Mono sorted_monos[]) {
    size_t real_size = 0;

    for (size_t i = 0; i < count; ++i) {
        // jezeli dlugosc dodanych jednomianow jest rowna 0
        // lub obecny jednomian ma inny wykladnik (zawsze wiekszy) od ostatniego dodanego jednomianu
        // to przestawiamy obecny jednomian w tablicy sorted_monos na indeks real_size
        if (real_size == 0 || MonoGetExp(&sorted_monos[i]) != MonoGetExp(&sorted_monos[real_size - 1])) {
            sorted_monos[real_size++] = sorted_monos[i];
        }
        else {
            // dodajemy 2 jednomiany o tym samym wykladniku
            poly_exp_t exp = MonoGetExp(&sorted_monos[i]);
            Poly sum = PolyAdd(&sorted_monos[real_size - 1].p, &sorted_monos[i].p);

            MonoDestroy(&sorted_monos[i]);
            MonoDestroy(&sorted_monos[real_size - 1]);

            if (PolyIsZero(&sum)) {
                real_size--;
                PolyDestroy(&sum);
            }
            else
                sorted_monos[real_size - 1] = MonoFromPoly(&sum, exp);
        }
    }

    poly_coeff_t coeff;
    if (real_size == 0) {
        free(sorted_monos);
        sorted_monos = NULL;
        return PolyZero();
    }
    else if (real_size == 1 && MonoIsCoeff(&sorted_monos[0], &coeff)) {
        free(sorted_monos);
        sorted_monos = NULL;
        return PolyFromCoeff(coeff);
    }

    sorted_monos = (Mono*)realloc(sorted_monos, real_size * sizeof(Mono));
    CheckPtr(sorted_monos);
    return (Poly) {.arr = sorted_monos, .size = real_size};
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    Mono *sorted_monos = (Mono*) calloc(count, sizeof(Mono));
    CheckPtr(sorted_monos);
    memcpy(sorted_monos, monos, count * sizeof(Mono));
    qsort(sorted_monos, count, sizeof(Mono), CompareMonosByExp);

    return PolyAddMonosHelper(count, sorted_monos);
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    qsort(monos, count, sizeof(Mono), CompareMonosByExp);
    return PolyAddMonosHelper(count, monos);
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    Mono *sorted_monos = (Mono*) calloc(count, sizeof(Mono));
    for (size_t i = 0; i < count; ++i) {
        sorted_monos[i] = MonoClone(&monos[i]);
    }
    qsort(sorted_monos, count, sizeof(Mono), CompareMonosByExp);
    return PolyAddMonosHelper(count, sorted_monos);
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
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

/**
 * Funkcja pomocnicza do znajdowania stopnia wielomianu
 * przechodzi wielomian rekurencyjnie za każdym razem zwiekszając aktualny stopień
 * o wykładnik jednomianu
 * @param[in] p : wielomian
 * @param[in] deg_so_far : stopień wielomianu policzony do miejsca, w którym aktualnie jesteśmy
 * @return stopień wielomianu @p p
 */
static poly_exp_t PolyDegHelper(const Poly *p, poly_exp_t deg_so_far) {
    if (PolyIsCoeff(p))
        return deg_so_far;

    poly_exp_t deg = 0;
    for (size_t i = 0; i < PolyGetSize(p); ++i)
        deg = max(deg, PolyDegHelper(&p->arr[i].p, deg_so_far + MonoGetExp(&p->arr[i])));

    return deg;
}

poly_exp_t PolyDeg(const Poly *p) {
    assert(p != NULL);
    if (PolyIsZero(p))
        return -1;
    return PolyDegHelper(p, 0);
}

/**
 * Funkcja pomocnicza do znajdowania stopnia wielomianu ze względu na zadaną zmienną
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej, dla której ma być obliczony stopień
 * @param[in] current_index : indeks zmiennej, w której aktualnie jesteśmy
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
static poly_exp_t PolyDegByHelper(const Poly *p, size_t var_idx, size_t current_index) {
    assert(p != NULL);
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
    assert(p != NULL);
    if (PolyIsZero(p))
        return -1;
    return PolyDegByHelper(p, var_idx, 0);
}

/**
 * Funkcja pomocnicza do obliczania wielomianu przeciwnego
 * przechodzi wielomian rekurencyjnie, jak dochodzi do wielomianu stałego to zmienia jego znak na przeciwny
 * @param[in, out] clone : wielomian
 */
static void PolyNegHelper(Poly *clone) {
    if (PolyIsCoeff(clone)) {
        clone->coeff *= (-1);
        return;
    }

    for (size_t i = 0; i < PolyGetSize(clone); ++i)
        PolyNegHelper(&clone->arr[i].p);
}

Poly PolyNeg(const Poly *p) {
    assert(p != NULL);
    if (PolyIsZero(p))
        return PolyZero();
    Poly clone = PolyClone(p);
    PolyNegHelper(&clone);
    return clone;
}

Poly PolySub(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    Poly neg_q = PolyNeg(q);
    Poly result = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return result;
}

static Poly PolyMulPolyCoeff(const Poly *p, poly_coeff_t q_coeff) {
    if (q_coeff == 0)
        return PolyZero();
    else if (q_coeff == 1)
        return PolyClone(p);

    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff * q_coeff);

    Poly new_poly = PolyAlloc(PolyGetSize(p));
    size_t real_size = 0;

    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        Poly multiplied = PolyMulPolyCoeff(&p->arr[i].p, q_coeff);
        // jezeli otrzymany wielomian jest zerem to pomijamy go
        if (!PolyIsZero(&multiplied))
            new_poly.arr[real_size++] = MonoFromPoly(&multiplied, MonoGetExp(&p->arr[i]));
    }

    PolyRealloc(&new_poly, real_size);
    return new_poly;
}

static Poly PolyMulPoly(const Poly *p, const Poly *q) {
    Mono *monos = (Mono*)calloc(PolyGetSize(p) * PolyGetSize(q), sizeof(Mono));
    size_t real_size = 0;

    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        for (size_t j = 0; j < PolyGetSize(q); ++j) {
            Poly multiplied = PolyMul(&p->arr[i].p, &q->arr[j].p);
            // jezeli otrzymany wielomian jest zerem to pomijamy go
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
    CheckPtr(monos);
    Poly result = PolyAddMonos(real_size, monos);
    free(monos);
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    assert(p != NULL && q != NULL);
    if (PolyIsCoeff(p))
        return PolyMulPolyCoeff(q, p->coeff);
    else if (PolyIsCoeff(q))
        return PolyMulPolyCoeff(p, q->coeff);
    else
        return PolyMulPoly(p, q);
}


Poly PolyAt(const Poly *p, poly_coeff_t x) {
    assert(p != NULL);
    if (PolyIsCoeff(p))
        return PolyClone(p);

    Poly result = PolyZero();
    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        // obliczamy x_0 do potegi jaka przy nim stoi
        poly_coeff_t value = Power(x, MonoGetExp(&p->arr[i]));

        // mnozymy otrzymana wyzej wartosc przez wielomian z jednomianu, w ktorym jestesmy
        Poly multiplied = PolyMulPolyCoeff(&p->arr[i].p, value);

        // dodajemy otrzymana wartosc do dotychczasowego wyniku
        Poly add = PolyAdd(&multiplied, &result);

        PolyDestroy(&multiplied);
        PolyDestroy(&result);

        // aktualizujemy wynik
        result = add;
    }
    return result;
}

Poly PolyComposeHelper(const Poly *p, size_t next, size_t k, const Poly q[]) {
    if (PolyIsCoeff(p))
        return *p;

    /*
    if (next >= k) {
        return PolyZero();
    }  */

    Poly zero = PolyZero();
    Poly res = PolyZero();

    for (size_t i = 0; i < PolyGetSize(p); ++i) {
        Poly pow = PolyPower((next < k) ? &q[next] : &zero, MonoGetExp(&p->arr[i]));

        Poly compose_inside = PolyComposeHelper(&p->arr[i].p, next + 1, k, q);

        Poly cur_res = PolyMul(&pow, &compose_inside);

        PolyDestroy(&compose_inside);
        PolyDestroy(&pow);

        Poly add = PolyAdd(&cur_res, &res);

        PolyDestroy(&res);
        PolyDestroy(&cur_res);

        res = add;
    }
    return res;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    return PolyComposeHelper(p, 0, k, q);
}