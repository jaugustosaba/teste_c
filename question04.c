#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#define SYNTAX_ERROR "syntax error"

struct node {
    struct node* next;
    int          value;
};

struct linkedlist {
    struct node* head;
};

struct input {
    int                 len;
    struct linkedlist*  lists;
};


static
int pushat(struct node** head, int value) {
    struct node* n = malloc(sizeof(struct node));
    if (!n)
        return errno;
    n->next = *head;
    n->value = value;
    *head = n;

    return 0;
}

static
int pushordered(struct linkedlist* list, int value) {
    struct node* prev = NULL;
    struct node* n = list->head;
    while (n && (n->value > value)) {
        prev = n;
        n = n->next;
    }
    struct node** target = prev ? &prev->next : &list->head;
    return pushat(target, value);
}

static
void droplist(struct linkedlist* list) {
    struct node* h = list->head;
    while (h) {
        struct node* n = h;
        h = h->next;
        free(n);
    }
    list->head = NULL;
}

static
void skipspaces(FILE* fp) {
    int c = fgetc(fp);
    while ((c != EOF) && isspace(c)) {
        c = fgetc(fp);
    }
    if (c != EOF)
        ungetc(c, fp);
}

static
int parsech(FILE* fp, int c, const char** errmsg) {
    skipspaces(fp);
    if (fgetc(fp) != c) {
        *errmsg = SYNTAX_ERROR;
        return 1;
    }
    return 0;
}

static
int parsenum(FILE* fp, int* num, const char** errmsg) {
    skipspaces(fp);
    int c = fgetc(fp);
    if (!isdigit(c)) {
        *errmsg = SYNTAX_ERROR;
        return 1;
    }
    int n = c - '0';
    c = fgetc(fp);
    while ((c != EOF) && isdigit(c)) {
        n = 10*n + (c - '0');
        c = fgetc(fp);
    }
    if (c != EOF)
        ungetc(c, fp);
    *num = n;
    return 0;
}

static inline
int peekc(FILE* fp) {
    skipspaces(fp);
    int c = fgetc(fp);
    ungetc(c, fp);
    return c;
}


static
int parselist(FILE* fp, struct linkedlist* out, const char** errmsg) {
    struct linkedlist l = { .head = NULL };
    int err = parsech(fp, '[', errmsg);
    if (err)
        goto error;
    
    if (peekc(fp) != ']') {
        int num = 0;
        err = parsenum(fp, &num, errmsg);
        if (err)
            goto error;
        err = pushordered(&l, num);
        if (err)
            goto error;

        while (peekc(fp) == ',') {
            err = parsech(fp, ',', errmsg);
            assert(!err);

            num = 0;
            err = parsenum(fp, &num, errmsg);
            if (err)
                goto error;
            err = pushordered(&l, num);
            if (err)
                goto error;
        }
    }

    err = parsech(fp, ']', errmsg);
    if (err)
        goto error;

    *out = l;
    return 0;

error:
    droplist(&l);
    return 1;
}

static
int parselists(FILE* fp, struct input* in, const char** errmsg);

static
int parseliststail(FILE* fp, struct input* in, const char** errmsg) {
    if (peekc(fp) == ']') {
        in->lists = calloc(in->len, sizeof(struct linkedlist*));
        if (!in->lists)
            return 1;
        return 0;
    } 

    int err = parsech(fp, ',', errmsg);
    if (err)
        return 1;

    return parselists(fp, in, errmsg);
}

int parselists(FILE* fp, struct input* in, const char** errmsg) {
    struct linkedlist l = { 0 };
    int err = parselist(fp, &l, errmsg);
    if (err)
        return 1;

    int i = in->len++;
    err = parseliststail(fp, in, errmsg);
    if (err) {
        droplist(&l);
        return 1;
    }
    in->lists[i] = l;

    return 0;
}

static
void dropinput(struct input* in) {
    for (int i=0; i < in->len; i++) {
        droplist(&in->lists[i]);
    }
    free(in->lists);
    in->len = 0;
    in->lists = NULL;
}

static
int parse(FILE* fp, struct input* in, const char** errmsg) {
    int err = parsech(fp, '[', errmsg);
    if (err)
        goto error;
    
    err = parselists(fp, in, errmsg);
    if (err)
        goto error;

    err = parsech(fp, ']', errmsg);
    if (err)
        goto error;

    err = parsech(fp, EOF, errmsg);
    if (err)
        goto error;

    return 0;

error:
    if (in->lists)
        dropinput(in);
    return 1;
}

static
void merge(struct input* in, struct linkedlist* out) {
    while (1) {
        struct linkedlist* max = NULL;
        for (int i=0; i < in->len; i++) {
            struct node* h = in->lists[i].head;
            if (h && (!max || (h->value > max->head->value)))
                max = &in->lists[i];
        }
        if (!max)
            return;
        
        struct node* n = max->head;
        max->head = n->next;

        n->next = out->head;
        out->head = n;
    }
}

static
void printlist(const struct linkedlist* l) {
    printf("[");
    const struct node* n = l->head;
    while (n) {
        printf("%d", n->value);
        n = n->next;
        if (n) {
            printf(", ");
        }
    }
    printf("]");
}

int main(int argc, char* argv[]) {
    const char *errmsg = NULL;
    struct input in = { 0 };

    int err = parse(stdin, &in, &errmsg);
    if (err) {
        if (!errmsg) {
            errmsg = strerror(errno);
        }
        fprintf(stderr, "%s", errmsg);
        return 1;
    }

    struct linkedlist out = { 0 };
    merge(&in, &out);
    printlist(&out);
    puts("\n");

    droplist(&out);
    dropinput(&in);

    return 0;
}