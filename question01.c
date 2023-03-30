#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

struct input {
    size_t  length;
    size_t  position;
    char    data[0];
};

struct word {
    const char* data;
    size_t len;
};

static
void usage(const char *exename) {
    printf("usage:\n\n");
    printf("  %s FILE\n\n", exename);
    exit(1);
}


static
struct input* loadfile(const char* filename) {
    struct input* in = NULL;
    FILE* fp = fopen(filename, "r");
    if (!fp)
        goto cleanup;
    int err = fseek(fp, 0, SEEK_END);
    if (err)
        goto cleanup;
    long length = ftell(fp);
    if (length < 0)
        goto cleanup;
    err = fseek(fp, 0, SEEK_SET);
    if (err)
        goto cleanup;
    in = malloc(sizeof(struct input) + sizeof(char) * length);
    if (!in)
        goto cleanup;
    in->length = length;
    in->position = 0;
    size_t read = fread(in->data, sizeof(char), length, fp);
    assert(read == in->length);

    return in;
cleanup:
    if (in)
        free(in);
    if (fp)
        fclose(fp);

    return NULL;
}

static
inline int haschrs(struct input* in) {
    return in->position < in->length;
}

static
inline char peekchr(struct input* in) {
    assert(haschrs(in));
    return in->data[in->position];
}

static
inline void nextchr(struct input* in) {
    assert(haschrs);
    in->position++;
}

static
inline char* curptr(struct input* in) {
    assert(haschrs);
    return &in->data[in->position];
}

static
int readword(struct input* in, struct word* w) {
    while (haschrs(in) && isspace(peekchr(in)))
        nextchr(in);

    if (!haschrs(in))
        return 0;
    
    w->data = curptr(in);
    w->len = 0;
    while (haschrs(in) && !isspace(peekchr(in))) {
        w->len++;
        nextchr(in);
    }
    
    return 1;
}

static
int ispalindrome(const struct word* w) {
    for (size_t i = 0; i < (w->len+1)/2; i++) {
        if (w->data[i] != w->data[w->len-i-1])
            return 0;
    }
    return 1;
}

static
void checksubstrs(struct word* w) {
    struct word palindrome = { 0 };
    int found = 0;

    for (int i=0; i < w->len; i++) {
        for (int j=i+1; j < w->len; j++) {
            struct word subword = {
                .data = &w->data[i],
                .len = j - i, 
            };
            if (ispalindrome(&subword) && (!found || (palindrome.len < subword.len))) {
                palindrome = subword;
                found = 1;
            }
        }
    }

    printf("%.*s: ", w->len, w->data);
    if (found) {
        printf("%.*s (%d)", palindrome.len, palindrome.data, palindrome.len);
    } else {
        printf("not found!");
    }
    printf("\n");
}

static
void process(struct input* in) {
    struct word w = { 0 };

    while (readword(in, &w))
        checksubstrs(&w);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        assert(argc > 0);
        usage(argv[0]);
    }
    int status = 0;
    struct input* in = NULL;
 
    in = loadfile(argv[1]);
    if (!in)
        goto failure;
    process(in);

    goto cleanup;
failure:
    status = 0;
    fprintf(stderr, "error:\n\n");
    fprintf(stderr, "  %s\n\n", strerror(errno));

cleanup:
    if (in)
        free(in);
    return status;
}