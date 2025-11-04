#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Poly {
    int coeff;
    int power;
    struct Poly* next;
} Poly;

Poly* createPoly(int coeff, int power) {
    Poly* n = (Poly*)malloc(sizeof(Poly));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->coeff = coeff;
    n->power = power;
    n->next = NULL;
    return n;
}

void insertTerm(Poly** head, int coeff, int power) {
    if (coeff == 0) return;
    Poly* prev = NULL, * cur = *head;
    while (cur && cur->power > power) {
        prev = cur;
        cur = cur->next;
    }
    if (cur && cur->power == power) {
        cur->coeff += coeff;
        if (cur->coeff == 0) {
			// remove term
            if (prev) prev->next = cur->next;
            else *head = cur->next;
            free(cur);
        }
        return;
    }
    Poly* nw = createPoly(coeff, power);
    nw->next = cur;
    if (prev) prev->next = nw;
    else *head = nw;
}

void freePoly(Poly* head) {
    while (head) {
        Poly* t = head;
        head = head->next;
        free(t);
    }
}

void printPoly(Poly* p) {
    if (!p) {
        printf("0\n");
        return;
    }
    int first = 1;
    while (p) {
        int c = p->coeff;
        int pow = p->power;
        if (!first) {
            if (c >= 0) printf(" + ");
            else printf(" - ");
        }
        else {
            if (c < 0) printf("-");
        }
        int absC = c < 0 ? -c : c;
		// print term
        if (pow == 0) {
            printf("%d", absC);
        }
        else if (pow == 1) {
            if (absC == 1) printf("x");
            else printf("%dx", absC);
        }
        else {
            if (absC == 1) printf("x^%d", pow);
            else printf("%dx^%d", absC, pow);
        }
        first = 0;
        p = p->next;
    }
    printf("\n");
}

// parses a line into a polynomial
void parseLineToPoly(const char* line, Poly** poly) {
    char* buf = strdup(line);
    if (!buf) { perror("strdup"); exit(EXIT_FAILURE); }
    char* tok = strtok(buf, " \t\r\n");
    while (tok) {
        char* endptr;
        long coeff = strtol(tok, &endptr, 10);
        if (endptr == tok) break; // not-int -> end
        tok = strtok(NULL, " \t\r\n");
        if (!tok) {
            fprintf(stderr, "Neparan broj članova u liniji\n");
            break;
        }
        long power = strtol(tok, &endptr, 10);
        if (endptr == tok) {
            fprintf(stderr, "Neispravan eksponent preskočen.\n");
            tok = strtok(NULL, " \t\r\n");
            continue;
        }
        insertTerm(poly, (int)coeff, (int)power);
        tok = strtok(NULL, " \t\r\n");
    }
    free(buf);
}

Poly* addPolynomials(Poly* a, Poly* b) {
    Poly* res = NULL;
    Poly* p = a;
    while (p) {
        insertTerm(&res, p->coeff, p->power);
        p = p->next;
    }
    p = b;
    while (p) {
        insertTerm(&res, p->coeff, p->power);
        p = p->next;
    }
    return res;
}

Poly* multiplyPolynomials(Poly* a, Poly* b) {
    Poly* res = NULL;
    for (Poly* pa = a; pa; pa = pa->next) {
        for (Poly* pb = b; pb; pb = pb->next) {
            int c = pa->coeff * pb->coeff;
            int pw = pa->power + pb->power;
            insertTerm(&res, c, pw);
        }
    }
    return res;
}

int main(int argc, char* argv[]) {
    const char* filename = "polinomi.txt";
    if (argc >= 2) filename = argv[1];

    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Otvaranje datoteke");
        return 1;
    }

    char line[4096];
    Poly* p1 = NULL, * p2 = NULL;
	// read two non-empty lines
    int readCount = 0;
    while (readCount < 2 && fgets(line, sizeof(line), f)) {
		// skip empty lines
        int onlyws = 1;
        for (char* s = line; *s; ++s) if (!(*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')) { onlyws = 0; break; }
        if (onlyws) continue;
        if (readCount == 0) parseLineToPoly(line, &p1);
        else if (readCount == 1) parseLineToPoly(line, &p2);
        readCount++;
    }
    fclose(f);

    if (!p1) {
        printf("Prvi polinom je prazan ili nije pronađen\n");
        freePoly(p1); freePoly(p2);
        return 1;
    }
    if (!p2) {
        printf("Drugi polinom je prazan ili nije pronađen\n");
        freePoly(p1); freePoly(p2);
        return 1;
    }

    printf("Polinom 1: ");
    printPoly(p1);
    printf("Polinom 2: ");
    printPoly(p2);

    Poly* sum = addPolynomials(p1, p2);
    printf("\nZbroj: ");
    printPoly(sum);

    Poly* prod = multiplyPolynomials(p1, p2);
    printf("Umnozak: ");
    printPoly(prod);

    freePoly(p1); freePoly(p2); freePoly(sum); freePoly(prod);
    return 0;
}
