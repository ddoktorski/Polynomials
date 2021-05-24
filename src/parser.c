/** @file
  Implementacja funkcji do parsowania wielomianów

  Dariusz Doktorski <dd394248@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <limits.h>
#include "parser.h"
#include "memory_helper.h"

///@{
 /**
  * Stała używana w kodzie, zapisana w wygodny sposób.
  */
#define BASE 10
#define MIN_DIGIT '0'
#define MAX_DIGIT '9'
#define LEFT_BRACKET '('
#define RIGHT_BRACKET ')'
#define COMMA ','
#define PLUS '+'
#define MINUS '-'
#define HASH '#'
#define SPACE ' '
#define UNDERSCORE '_'
#define EOL '\n'
#define SMALL_A 'a'
#define SMALL_Z 'z'
#define BIG_A 'A'
#define BIG_Z 'Z'
///@}

 ///@{
/** Komenda dostępna w kalkulatorze wielomianów. */
#define ZERO "ZERO"
#define IS_COEFF "IS_COEFF"
#define IS_ZERO "IS_ZERO"
#define CLONE "CLONE"
#define ADD "ADD"
#define MUL "MUL"
#define SUB "SUB"
#define NEG "NEG"
#define IS_EQ "IS_EQ"
#define DEG "DEG"
#define DEG_BY "DEG_BY"
#define AT "AT"
#define PRINT "PRINT"
#define POP "POP"
///@}

/** Maksymalna wartość dla typu unsigned long long zapisana jako string. */
const char *ullong_max_string = "18446744073709551615";

String CreateString() {
    char *arr = (char*) calloc(INIT_SIZE, sizeof(char));
    CheckPtr(arr);
    return (String) {.arr = arr, .allocated_size = INIT_SIZE, .size = 0};
}

MonosArr CreateMonosArr() {
    Mono *arr = (Mono*) calloc(INIT_SIZE, sizeof(Mono));
    CheckPtr(arr);
    return (MonosArr) {.arr = arr, .allocated_size = INIT_SIZE, .size = 0};
}

/**
 * Ustawia wszystkie atrybuty obiektu ParseProtector na false.
 * @param[in] protector : informacje o stanie wczytywanego wiersza
 */
static void ResetParseProtector(ParserProtector *protector) {
    protector->error = false;
    protector->end_of_file = false;
    protector->end_of_line = false;
}

void DestroyString(String *s) {
    free(s->arr);
    s->arr = NULL;
}

void DestroyMonosArr(MonosArr *monos) {
    free(monos->arr);
    monos->arr = NULL;
}

void CheckStringSpace(String *str) {
    if (str->size == str->allocated_size) {
        str->allocated_size = IncreaseSpace(str->allocated_size);
        str->arr = (char*) realloc(str->arr, str->allocated_size * sizeof(char));
        CheckPtr(str->arr);
    }
}

void CheckMonosArrSpace(MonosArr *monos) {
    if (monos->size == monos->allocated_size) {
        monos->allocated_size = IncreaseSpace(monos->allocated_size);
        monos->arr = (Mono*) realloc(monos->arr, monos->allocated_size * sizeof(Mono));
        CheckPtr(monos->arr);
    }
}

/**
 * Sprawdza czy kolejna liczba do wczytania jest ujemna.
 * @return Czy kolejnym znak do wczytania jest ujemny?
 */
static bool CheckIfNumberIsNegative() {
    int sign = getchar();
    if (sign == MINUS)
        return true;
    else {
        ungetc(sign, stdin);
        return false;
    }
}

/**
 * Sprawdza czy wczytany współczynnik jest poprawny.
 * @param[in] n : współczynnik
 * @return Czy współczynnik jest poprawny?
 */
static inline bool CheckCoeffCorrect(Number *n) {
    // przypadek value = 0?
    return n->value <= (ull) (LONG_MAX + n->minus);
}

/**
 * Sprawdza, czy wczytany wykładnik ma poprawną wartość.
 * @param[in] n : wykładnik
 * @return Czy wykładnik ma poprawną wartość?
 */
static inline bool CheckExpCorrect(Number *n) {
    return !n->minus && n->value <= INT_MAX;
}

/**
 * Sprawdza czy wczytany argument polecenia DEG_BY jest poprawny.
 * @param[in] n : argument polecenia DEG_BY
 * @return Czy argument polecenia DEG_BY jest poprawny?
 */
static inline bool CheckArgDegBy(Number *n) {
    return !n->minus && n->value <= ULLONG_MAX;
}

/**
 * Sprawdza czy liczba przechowywana jako napis w @p s jest wartości ULLONG_MAX.
 * @param[in] s : napis będący liczbą
 * @param[in] number : liczba otrzymana w wyniku zamiany napisu @p s na liczbę funkcją stroull
 * @param[in,out] error : informacja o błędzie
 */
static void CheckIfLlongMax(String *s, ull number, bool *error) {
    if (number == ULLONG_MAX) {
        if (strcmp(s->arr, ullong_max_string) != 0)
            *error = true;
    }

}

Number ParseNumber(bool *error) {
    String str = CreateString();
    bool minus = CheckIfNumberIsNegative();

    int digit;
    while ((digit = getchar()) >= MIN_DIGIT && digit <= MAX_DIGIT) {
        CheckStringSpace(&str);
        str.arr[str.size++] = (char) digit;
    }

    if (str.size == 0) {
        ungetc(digit, stdin);
        *error = true;
        return (Number) {.minus = false, .value = 0};
    }

    ungetc(digit, stdin); // zwracamy na standardowe wejście ostatni wczytany znak
    str.arr = (char*) realloc(str.arr, str.size);
    ull number = strtoull(str.arr, NULL, BASE);

    // strtoull zwraca ULLONG_MAX rowniez wtedy kiedy liczba w tablicy jest wieksza od ULLONG_MAX
    // zatem sprawdzamy jezeli number == ULLONG_MAX czy rzeczywiscie jest to ta wartoscia
    CheckIfLlongMax(&str, number, error);

    DestroyString(&str);

    return (Number) {.minus = minus, .value = number};
}

/**
 * Parsuje współczynnik jednomianu lub wartość argumentu funkcji AT.
 * W przypadku wystąpienia błędu zwraca domyślnie 0.
 * @param[in,out] error : informacja o błędzie
 * @return wartość współczynnika lub argumentu funkcji AT lub 0 w przypadku błędu
 */
poly_coeff_t ParseCoeff(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckCoeffCorrect(&num))
        *error = true;
    // w przypadku błędu zwracamy 0, aczkolwiek nie ma to żadnego znaczenia co byśmy zwrócili
    // informacja o błędzie jest zapisana w zmiennej error
    // w kolejnych 2 funkcjach postępuje tak samo
    return (*error) ? 0 : (long) ((num.minus) ? (-1) * num.value : num.value);
}

/**
 * Parsuje wykładnik jednomianu.
 * @param[in,out] error : informacja o błędzie
 * @return wartość wykładnika jednomianu lub 0 w przypadku błędu
 */
poly_exp_t ParseExp(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckExpCorrect(&num))
        *error = true;
    return (*error) ? 0 : (int) ((num.minus) ? (-1) * num.value : num.value);
}

/**
 * Parsuje argument polecenia DEG_BY.
 * @param[in,out] error : informacja o błędzie
 * @return wartość argumentu polecenia DEG_BY lub 0 w przypadku błędu
 */
ull ParseArgDegBy(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckArgDegBy(&num))
        *error = true;
    return (*error) ? 0 : num.value;
}

/**
 * Stwierdza czy wiersz jest już zakończony.
 * @param[in] protector : informacje o stanie parsowanego wiersza
 * @return Czy wiersz jest zakończony?
 */
static inline bool LineIsOver(ParserProtector *protector) {
    return protector->end_of_file || protector->end_of_line;
}

/**
 * Stwierdza czy parsowanie wiersza powinno zostać zakończone.
 * @param[in] protector : informacje o stanie parsowanego wiersza
 * @return Czy należy zakończyć parsowanie wiersza?
 */
static inline bool StopParsing(ParserProtector *protector) {
    return protector->end_of_file || protector->end_of_line || protector->error;
}

/**
 * Wczytuje znak ze standardowego wejścia zapisuje go i następnie oddaje ten
 * ponownie na standardowe wejście.
 * @return kolejny do wczytania znak
 */
static int NextChar() {
    int next = getchar();
    ungetc(next, stdin);
    return next;
}

/**
 * Jeżeli nie został osiągnięty konieć wiersza to sprawdza czy kolejny znak
 * jest końcem linii bądź końcem pliku.
 * @param[in,out] protector : informacje o stanie parsowanego wiersza
 */
static void CheckIfEnd(ParserProtector *protector) {
    if (!LineIsOver(protector)) {
        int next = getchar();
        if (next == EOL)
            protector->end_of_line = true;
        else if (next == EOF)
            protector->end_of_file = true;
        else
            ungetc(next, stdin);
    }
}

/**
 * Jeżeli parsowanie wiersza nie zostało zakończone to wczytuje kolejny znak i sprawdza
 * czy jego wartość jest równa @p value, jeżeli nie to zapisuje informacje o błędzie.
 * @param[in] value : oczekiwana wartość kolejnego znaku (zapisana kodem ASCII)
 * @param[in,out] protector : informacje o stanie wczytywanego wiersza
 */
static void CheckNextChar(int value, ParserProtector *protector) {
    CheckIfEnd(protector);
    if (!StopParsing(protector)) {
        int next = getchar();
        if (next != value)
            protector->error = true;
    }
}

/**
 * Wczytuje kolejne znak aż do znaku nowej linii lub końca pliku.
 * @param[in,out] protector : informacje o stanie wczytywanego wiersza
 */
static void SkipLine(ParserProtector *protector) {
    int c;
    do {
        c = getchar();
    } while (c != EOF && c != EOL);
    protector->end_of_file = (c == EOF) ? true : false;
    protector->end_of_line = (c == EOL) ? true : false;
}

/**
 * Sprawdza czy kolejny wielomian do wczytania jest współczynnikiem.
 * @return Czy kolejny wielomian do wczytania jest współczynnikiem?
 */
static bool NextPolyIsCoeff() {
    int next = NextChar();
    return next == MINUS || (MIN_DIGIT <= next && next <= MAX_DIGIT);
}

/**
 * Sprawdza czy wiersz jest komentarzem lub wierszem pustym.
 * @return Czy wiersz jest komentarzem lub wierszem pustym?
 */
static bool CommentOrEmptyLine() {
    int next = NextChar();
    return next == EOL || next == HASH;
}

/**
 * Sprawdza czy wiersz jest poleceniem.
 * @return Czy następny wczytywany znak jest literą?
 */
static bool CommandInLine() {
    int next = NextChar();
    return (SMALL_A <= next && next <= SMALL_Z) || (BIG_A <= next && next <= BIG_Z);
}

/**
 * Sprawdza czy @p sign jest wielką literą.
 * @param[in] sign : znak (wartość znaku zapisana w kodzie ASCII)
 * @return Czy @p sign jest wielką literą?
 */
static bool IsLetter(int sign) {
    return BIG_A <= sign && sign <= BIG_Z;
}

/**
 * Sprawdza czy kolejny znak do wczytania jest spacją, jeżeli nie to oddaje
 * ten znak na standardowe wejście.
 * @return Czy kolejny znak jest spacją?
 */
static bool NextIsSpace() {
    int next = getchar();
    if (next == SPACE) {
        return true;
    }
    else {
        ungetc(next, stdin);
        return false;
    }
}

/**
 * Sprawdza czy doszliśmy do końca wielomianu. Jeżeli kolejny znak jest plusem to go pobiera, jeżeli
 * jest przecinkiem to znaczy, że doszliśmy do końca wielomianu, natomiast jeżeli jest to inny znak to
 * otrzymujemy błąd.
 * @param[in,out] error : informacja o błędzie
 * @param[in,out] end_of_poly : informacja o tym, czy doszliśmy do końca wielomianu
 */
static void CheckIfEndOfPoly(bool *error, bool *end_of_poly) {
    int next = NextChar();
    if (next == PLUS)
        getchar();
    else if (next == COMMA)
        *end_of_poly = true;
    else
        *error = true;
}

Mono ParseMono(ParserProtector *protector) {
    CheckNextChar(LEFT_BRACKET, protector);

    if (StopParsing(protector)) {
        protector->error = true;
        return (Mono) {.p = PolyZero(), .exp = 1};
    }

    Poly p = ParsePoly(protector);

    CheckNextChar(COMMA, protector);

    if (StopParsing(protector)) {
        protector->error = true;
        return (Mono) {.p = PolyZero(), .exp = 10};
    }

    poly_exp_t exp = ParseExp(&protector->error);

    CheckNextChar(RIGHT_BRACKET, protector);

    if (StopParsing(protector)) {
        protector->error = true;
        return (Mono) {.p = PolyZero(), .exp = 10};
    }

    return MonoFromPoly(&p, exp);
}

Poly ParsePoly(ParserProtector *protector) {
    if (NextPolyIsCoeff()) {
        poly_coeff_t coeff = ParseCoeff(&protector->error);
        return PolyFromCoeff(coeff);
    }

    CheckIfEnd(protector);
    MonosArr monos = CreateMonosArr();

    // ta zmienna kontroluje, czy nie doszliśmy do końca wielomianu, wtedy gdy ten koniec jest przecinkiem
    // oddzielającym wielomian od wykładnika w jednomianie
    bool end_of_poly = false;

    while (!StopParsing(protector) && !end_of_poly) {
        CheckMonosArrSpace(&monos);
        Mono m = ParseMono(protector);
        if (!PolyIsZero(&m.p))
            monos.arr[monos.size++] = m;

        CheckIfEnd(protector);

        // jeżeli nie mamy powodu żeby zakończyć parsowanie
        // to kolejny znak musi być plusem lub przecinkiem
        // jeżeli jest plusem to kontunuujemy parsowanie
        // jeżeli jest przecinkiem to kończymy
        if (!StopParsing(protector))
            CheckIfEndOfPoly(&protector->error, &end_of_poly);
    }

    Poly p = PolyAddMonos(monos.size, monos.arr);
    DestroyMonosArr(&monos);
    return p;
}

/**
 * Sprawdza czy napis @p command jest równy tablicy znaków @p arr.
 * @param[in] command : napis
 * @param[in] arr : tablica znaków
 * @return Czy oba parametry przechowują ten sam napis?
 */
static inline bool CommandsEqual(String *command, char *arr) {
    return strcmp(command->arr, arr) == 0;
}

void ParseCommand(String *command) {
    int next = getchar();
    while ((IsLetter(next) || next == UNDERSCORE) && command->size < 10) {
        CheckStringSpace(command);
        command->arr[command->size++] = (char) next;
        next = getchar();
    }
    ungetc(next, stdin);
    CheckStringSpace(command);
    command->arr[command->size] = '\0';
}

void ExecuteCommand(Stack *s, String *command, ParserProtector *protector, size_t row) {
    CheckIfEnd(protector);

    if (CommandsEqual(command, DEG_BY)) {
        int next = NextChar();
        if (LineIsOver(protector) || (9 <= next && next <= 13)) {
            protector->error = true;
            ErrorDegBy(row);
            return;
        }
        if (!NextIsSpace()) {
            protector->error = true;
            ErrorWrongCommand(row);
            return;
        }

        ull arg = ParseArgDegBy(&protector->error);
        CheckIfEnd(protector);

        if (!LineIsOver(protector) || protector->error) {
            ErrorDegBy(row);
            return;
        }
        DegBy(s, row, arg);
    }
    else if (CommandsEqual(command, AT)) {
        int next = NextChar();
        // wnioskuje z przykładowych testów, że jeżeli po poleceniu mamy biały znak inny niż
        // spacja to traktujemy to jako błąd argumentu, natomiast jeżeli mamy jakiś inny znak
        // to wówczas jest to błąd polecenia
        // dla komendy AT postępuje tak samo
        if (LineIsOver(protector) || (9 <= next && next <= 13)) {
            protector->error = true;
            ErrorAt(row);
            return;
        }
        if (!NextIsSpace()) {
            protector->error = true;
            ErrorWrongCommand(row);
            return;
        }

        poly_coeff_t x = ParseCoeff(&protector->error);
        CheckIfEnd(protector);

        if (!LineIsOver(protector) || protector->error) {
            protector->error = true;
            ErrorAt(row);
            return;
        }
        At(s, row, x);
    }
    else if (LineIsOver(protector)) {
        if (CommandsEqual(command, ZERO))
            Zero(s);
        else if (CommandsEqual(command, IS_COEFF))
            IsCoeff(s, row);
        else if (CommandsEqual(command, IS_ZERO))
            IsZero(s, row);
        else if (CommandsEqual(command, CLONE))
            Clone(s, row);
        else if (CommandsEqual(command, ADD))
            Add(s, row);
        else if (CommandsEqual(command, MUL))
            Mul(s, row);
        else if (CommandsEqual(command, SUB))
            Sub(s, row);
        else if (CommandsEqual(command, NEG))
            Neg(s, row);
        else if (CommandsEqual(command, IS_EQ))
            IsEq(s, row);
        else if (CommandsEqual(command, DEG))
            Deg(s, row);
        else if (CommandsEqual(command, POP))
            Pop(s, row);
        else if (CommandsEqual(command, PRINT))
            Print(s, row);
        else {
            protector->error = true;
            ErrorWrongCommand(row);
        }
    }
    else {
        protector->error = true;
        ErrorWrongCommand(row);
    }
}

void ParseInput(Stack *s) {
    ParserProtector protector;
    ResetParseProtector(&protector);
    String command = CreateString();
    size_t row_number = 1;

    while (!protector.end_of_file) {
        ResetParseProtector(&protector);
        if (!CommentOrEmptyLine()) {
            if (CommandInLine()) {
                ParseCommand(&command);
                ExecuteCommand(s, &command, &protector, row_number);
                command.size = 0; // resetujemy długość napisu
            }
            else { // parsujemy wielomian
                Poly p = ParsePoly(&protector);
                CheckIfEnd(&protector);
                if (protector.error || !LineIsOver(&protector)) {
                    PolyDestroy(&p);
                    ErrorWrongPoly(row_number);
                }
                else {
                    StackPush(s, &p);
                }
            }
        }

        // jeżeli nie doszliśmy do końca linii do wczytujemy pozostałe znaki
        if (!LineIsOver(&protector))
            SkipLine(&protector);

        if (!protector.end_of_file)
            CheckIfEnd(&protector);

        row_number++;
    }
    DestroyString(&command);
}


