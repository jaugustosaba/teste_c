#include <stdio.h>

#define MAX_NUMERALS 7

struct roman {
    char letter;
    int  value;
};

static
const struct roman VALUES[] = {
    {'I', 1},
    {'V', 5},
    {'X', 10},
    {'L', 50},
    {'C', 100},
    {'D', 500},
    {'M', 1000},
};

static
int getvidx(char c) {
    for (int i=0; i < MAX_NUMERALS; i++) {
        if (VALUES[i].letter == c)
            return i;
    }
    return -1;
}

static
int convert(const char* s, int *out) {
    int acc = 0;
    while (*s) {
        int c = *s++;
        int i = getvidx(c);
        if (i < 0)
            return 1;
        int v = VALUES[i].value;
        if (*s) {
            int j = getvidx(*s);
            if (j - i >= 1) {
                s++;
                v = VALUES[j].value - v;
            }
        }
        acc += v;
    }
    *out = acc;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage:\n\n");
        fprintf(stderr, "  %s ROMAN\n", argv[0]);
        return 1;
    }
    const char* numeral = argv[1];
    int value = 0;
    int err = convert(numeral, &value);
    if (err) {
        fprintf(stderr, "invalid numeral: %s", numeral);
        return 2;
    }
    printf("%d\n", value);
    return 0;
}