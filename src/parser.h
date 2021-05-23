#ifndef POLYNOMIALS_PARSER_H
#define POLYNOMIALS_PARSER_H

#include "stack.h"
#include "calc.h"
#include <string.h>

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

typedef unsigned long long ull;

typedef struct String{
    char *arr;
    size_t size;
    size_t allocated_size;
} String;

typedef struct Number {
    bool minus;
    ull value;
} Number;

typedef struct MonosArr {
    Mono *arr;
    size_t size;
    size_t allocated_size;
} MonosArr;

typedef struct ParserProtector {
    bool error;
    bool end_of_file;
    bool end_of_line;
} ParserProtector;


String CreateString();

Mono ParseMono(ParserProtector*);
Poly ParsePoly(ParserProtector*);
void ParseInput(Stack *s);

#endif //POLYNOMIALS_PARSER_H
