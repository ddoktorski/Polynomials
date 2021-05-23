#include <stdio.h>
#include "limits.h"
#include "parser.h"
#include "memory_handling.h"

#define MIN_DIGIT 48
#define MAX_DIGIT 57

#define LEFT_BRACKET 40
#define RIGHT_BRACKET 41

#define BASE 10

#define COMMA 44
#define PLUS 43
#define MINUS 45
#define HASH 35
#define SPACE 32

#define EOL 10

#define SMALL_A 97
#define SMALL_Z 122

#define BIG_A 65
#define BIG_Z 90

#define LETTER_SHIFT 32

const char *ullong_max_string = "18446744073709551615";

String CreateString() {
    char *arr = (char*) calloc(INIT_SIZE, sizeof(char));
    CheckPtr(arr);
    return (String) {.arr = arr, .allocated_size = INIT_SIZE, .size = 0};
}

void ReallocString(String *str) {
    if (str->size != 0) {
        str->arr = (char*) realloc(str->arr, str->size * sizeof(char));
        CheckPtr(str->arr);
    }
}

MonosArr CreateMonosArr() {
    Mono *arr = (Mono*) calloc(INIT_SIZE, sizeof(Mono));
    CheckPtr(arr);
    return (MonosArr) {.arr = arr, .allocated_size = INIT_SIZE, .size = 0};
}

ParserProtector InitParserProtector() {
    return (ParserProtector) {.error = false, .end_of_file = false, .end_of_line = false};
}

static void ResetParseProtector(ParserProtector *protector) {
    protector->error = false;
    protector->end_of_file = false;
    protector->end_of_line = false;
}

static void DestroyString(String *s) {
    free(s->arr);
    s->arr = NULL;
}

static void DestroyMonosArrShallow(MonosArr *monos) {
    free(monos->arr);
    monos->arr = NULL;
}

static void DestroyMonosArrDeep(MonosArr *monos) {
    for (size_t i = 0; i < monos->size; ++i)
        MonoDestroy(&monos->arr[i]);
    DestroyMonosArrShallow(monos);
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

static bool CheckIfNumberIsNegative() {
    int sign = getchar();
    if (sign == MINUS)
        return true;
    else {
        ungetc(sign, stdin);
        return false;
    }
}

static inline bool CheckCoeffCorrect(Number *n) {
    // przypadek value = 0?
    return n->value <= (ull) (LONG_MAX + n->minus);
}

static inline bool CheckExpCorrect(Number *n) {
    return !n->minus && n->value <= INT_MAX;
}

static inline bool CheckArgDegBy(Number *n) {
    return !n->minus && n->value <= ULLONG_MAX;
}

static void CheckIfLlongMax(String *s, bool minus, bool *error) {
    *error = !minus && (strcmp(s->arr, ullong_max_string) == 0);
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
    CheckIfLlongMax(&str, minus, error);

    DestroyString(&str);

    return (Number) {.minus = minus, .value = number};
}

ull ConvertNumber(Number *n) {
    return (n->minus) ? (-1) * n->value : n->value;
}

poly_coeff_t ParseCoeff(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckCoeffCorrect(&num))
        *error = true;
    return (*error) ? 0 : (long) ConvertNumber(&num);
}

poly_exp_t ParseExp(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckExpCorrect(&num))
        *error = true;
    return (*error) ? 0 : (int) ConvertNumber(&num);
}

ull ParseArgDegBy(bool *error) {
    Number num = ParseNumber(error);
    if (!CheckArgDegBy(&num))
        *error = true;
    return (*error) ? 0 : ConvertNumber(&num);
}

static void CheckIfEnd(ParserProtector *protector) {
    int next = getchar();
    if (next == EOL)
        protector->end_of_line = true;
    else if (next == EOF)
        protector->end_of_file = true;
    else
        ungetc(next, stdin);
}

static inline bool StopParsing(ParserProtector *protector) {
    return protector->end_of_file || protector->end_of_line || protector->error;
}

static void CheckNextChar(int value, ParserProtector *protector) {
    CheckIfEnd(protector);
    if (!StopParsing(protector)) {
        int next = getchar();
        protector->error = next != value;
    }
}

static void SkipLine(ParserProtector *protector) {
    int c;
    do {
        c = getchar();
    } while (c != EOF && c != EOL);
    protector->end_of_file = (c == EOF) ? true : false;
}

Mono ParseMono(ParserProtector *protector) {
    CheckNextChar(LEFT_BRACKET, protector);
    Poly p = ParsePoly(protector);
    CheckNextChar(COMMA, protector);
    poly_exp_t exp = ParseExp(&protector->error);
    CheckNextChar(RIGHT_BRACKET, protector);
    return MonoFromPoly(&p, exp);
}

static int NextChar() {
    int next = getchar();
    ungetc(next, stdin);
    return next;
}

static bool NextPolyIsCoeff() {
    int next = NextChar();
    return next == MINUS || (MIN_DIGIT <= next && next <= MAX_DIGIT);
}

Poly ParsePoly(ParserProtector *protector) {
    if (NextPolyIsCoeff()) {
        poly_coeff_t coeff = ParseCoeff(&protector->error);
        return PolyFromCoeff(coeff);
    }

    CheckIfEnd(protector);
    MonosArr monos = CreateMonosArr();

    while (!StopParsing(protector)) {
        CheckMonosArrSpace(&monos);
        Mono m = ParseMono(protector);
        CheckNextChar(PLUS, protector);
        monos.arr[monos.size++] = m;
        CheckIfEnd(protector);
    }

    /*
    if (protector->error) {
        DestroyMonosArrDeep(&monos);
        return PolyFromCoeff(1);
    }*/

    Poly p = PolyAddMonos(monos.size, monos.arr);
    DestroyMonosArrShallow(&monos);
    return p;
}

static bool CommentOrEmptyLine() {
    int next = NextChar();
    return next == EOL || next == HASH;
}

static bool PolyInLine() {
    int next = NextChar();
    return next == LEFT_BRACKET || next == MINUS || (MIN_DIGIT <= next && next <= MAX_DIGIT);
}

static bool IsLetter(int sign) {
    return (SMALL_A <= sign && sign <= SMALL_Z) || (BIG_A <= sign && sign <= BIG_Z);
}

String ParseCommand() {
    String command = CreateString();
    int next = getchar();
    while (IsLetter(next)) {
        CheckStringSpace(&command);
        if (SMALL_A <= next && next <= SMALL_Z)
            next -= LETTER_SHIFT; // jeżeli wczytamy małą literę to zamieniamy ją na wielką
        command.arr[command.size++] = (char) next;
        next = getchar();
    }
    ungetc(next, stdin);
    ReallocString(&command);
    return command;
}

static inline bool CommandsEqual(String *command, char *arr) {
    return strcmp(command->arr, arr) == 0;
}


void ExecuteCommand(Stack *s, String *command, ParserProtector *protector, size_t row) {
    if (CommandsEqual(command, ZERO)) {
        Zero(s);
    }
    else if (CommandsEqual(command, IS_COEFF)) {
        IsCoeff(s, row);
    }
    else if (CommandsEqual(command, IS_ZERO)) {
        IsZero(s, row);
    }
    else if (CommandsEqual(command, CLONE)) {
        Clone(s, row);
    }
    else if (CommandsEqual(command, ADD)) {
        Add(s, row);
    }
    else if (CommandsEqual(command, MUL)) {
        Mul(s, row);
    }
    else if (CommandsEqual(command, SUB)) {
        Sub(s, row);
    }
    else if (CommandsEqual(command, NEG)) {
        Neg(s, row);
    }
    else if (CommandsEqual(command, IS_EQ)) {
        IsEq(s, row);
    }
    else if (CommandsEqual(command, DEG)) {
        Deg(s, row);
    }
    else if (CommandsEqual(command, DEG_BY)) {
        CheckNextChar(SPACE, protector);
        if (protector->error) {
            ErrorDegBy(row);
            return;
        }
        ull arg = ParseArgDegBy(&protector->error);
        if (protector->error) {
            ErrorDegBy(row);
            return;
        }
        DegBy(s, row, arg);
    }
    else if (CommandsEqual(command, AT)) {
        CheckNextChar(SPACE, protector);
        if (protector->error) {
            ErrorAt(row);
            return;
        }
        long int x = ParseCoeff(&protector->error);
        if (protector->error) {
            ErrorAt(row);
            return;
        }
        At(s, row, x);
    }
    else if (CommandsEqual(command, POP)) {
        Pop(s, row);
    }
    else if (CommandsEqual(command, PRINT)) {
        Print(s, row);
    }
    else {
        ErrorWrongCommand(row);
    }
}

void ParseInput(Stack *s) {
    ParserProtector protector = InitParserProtector();
    size_t row_number = 1;

    while (!protector.end_of_file) {
        ResetParseProtector(&protector);
        if (!CommentOrEmptyLine()) {
            if (PolyInLine()) {
                Poly p = ParsePoly(&protector);
                if (protector.error) {
                    PolyDestroy(&p);
                    ErrorWrongPoly(row_number);
                }
                else {
                    StackPush(s, &p);
                }
            }
            else { // parsujemy komende
                String command = ParseCommand();
                ExecuteCommand(s, &command, &protector, row_number);
            }
        }

        if (!protector.end_of_file && !protector.end_of_line)
            SkipLine(&protector);

        CheckIfEnd(&protector);
        row_number++;
    }
}


