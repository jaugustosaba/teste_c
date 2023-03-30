#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t RGB;

static inline
uint8_t getbyte(RGB rgb, unsigned byte) {
    uint32_t mask = 0xFF << 8*byte;
    return (rgb & mask) >> 8*byte;
}

static inline
RGB setbyte(RGB rgb, unsigned byte, uint8_t value) {
    RGB mask = ~(0xFF << 8*byte);
    return (rgb & mask) | value << 8*byte; 
}

static inline
int cvtcolor(const char* s, RGB* rgb) {
    unsigned int tmp;
    if (sscanf(s, "%x", &tmp) < 0)
        return 1;
    *rgb = tmp;
    return 0;
}

static 
int loadcolor(const char *s, RGB* rgb, const char* label) {
    int err = cvtcolor(s, rgb);
    if (err)
        fprintf(stderr, "%s - invalid color: %s\n", label, s);
    return err;
}

static
int loadint(const char *s, int* n, const char* label) {
    int err;
    if ((err = sscanf(s, "%d", n) < 0) || (n < 0))
        fprintf(stderr, "%s - invalid number: %s\n", label, s);
    return err;
}

static inline
void printcolor(RGB color) {
    printf("%08x\n", color);
}

static
void redrange(RGB start, RGB end, int steps) {
    printcolor(start);
    if (steps <= 0)
        return;
    int delta[4] = { 0, 0, 0, 0 };
    for (int i=0; i < 4; i++) {
        int from = getbyte(start, i);
        int to = getbyte(end, i);
        delta[i] = (from - to) / steps;
    }
    for (int i=0; i < steps-1; i++) {
        RGB color = start;
        for (int j=0; j < 4; j++) {
            int component = getbyte(start, j);
            color = setbyte(color, j, component + (i+1)*delta[j]);
        }
        printcolor(color);
    }
    printcolor(end);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage:\n\n");
        fprintf(stderr, "  %s STARTCOLOR ENDCOLOR STEPS\n\n", argv[0]);
        return 1;
    }
    int status = 0;
    RGB start = 0;
    int err = loadcolor(argv[1], &start, "STARTCOLOR");
    if (err)
        goto failure;
    RGB end = 0;
    err = loadcolor(argv[2], &end, "ENDCOLOR");
    if (err)
        goto failure;
    int steps = 0;
    err = loadint(argv[3], &steps, "STEPS");
    if (err)
        goto failure;
    redrange(start, end, steps);
    goto done;
failure:
    status = 2;
done:
    return status;
}