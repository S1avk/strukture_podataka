#define _CRT_SECURE_NO_WARNINGS


#include "receipts.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>


// Postavlja sva slova kako mala slova i uspoređuje jesu li ista slova (C->c)
static int strcasecmp(const char* a, const char* b)
{
    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb)
            return ca - cb;
        ++a;
        ++b;
    }
    return (char)tolower((unsigned char)*a) -
        (char)tolower((unsigned char)*b);
}

//upisuje iz tekstualnog filea u string
static char* strdup_safe(const char* s) {
    if (!s) return NULL;
    char* d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

//Uspoređuje datume
static int cmp_date(const char* a, const char* b) {
    return strcmp(a, b);
}

// Stvaramo nove elemente liste za račune
static Receipt* receipt_create(const char* date) {
    Receipt* r = malloc(sizeof(Receipt));
    if (!r) return NULL;
    strncpy(r->date, date, 11);
    r->date[10] = '\0';
    r->articles = NULL;
    r->next = NULL;
    return r;
}
// <kreiramo artikl i ubacujemo vrijednosti
static Article* article_create(const char* name, int qty, double price) {
    Article* a = malloc(sizeof(Article));
    if (!a) return NULL;
    a->name = strdup_safe(name);
    a->qty = qty;
    a->price = price;
    a->next = NULL;
    return a;
}

// Sortirani unos računa u listu po datumu (uzlazno)
static int insert_receipt_sorted(Receipt** head, Receipt* node) {
    if (!*head || cmp_date(node->date, (*head)->date) < 0) {
        node->next = *head;
        *head = node;
        return;
    }
    Receipt* cur = *head;
    while (cur->next && cmp_date(cur->next->date, node->date) <= 0) {
        cur = cur->next;
    }
    node->next = cur->next;
    cur->next = node;
    return 0;
}

// sortirani unos artikala u listu po imenu (case-insensitive)
static int insert_article_sorted(Article** head, Article* node) {
    if (!*head || strcasecmp(node->name, (*head)->name) < 0) {
        node->next = *head;
        *head = node;
        return 1;
    }
    Article* cur = *head;
    while (cur->next && strcasecmp(cur->next->name, node->name) <= 0) {
        cur = cur->next;
    }
    node->next = cur->next;
    cur->next = node;
    return 0;
}

//Miče prazne znakove s početka i kraja stringa
static int trim(char* s) {
    if (!s) return;
    // left
    char* p = s;
    while (isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    // right
    char* end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
	return 0;
}

// Učitava redak artikla i upisuje ga u ime, količinu i cijenu
static bool parse_article_line(const char* line, char** out_name, int* out_qty, double* out_price) {
    if (!line) return false;
    // copy line
    char* tmp = strdup_safe(line);
    if (!tmp) return false;
    trim(tmp);
    if (strlen(tmp) == 0) { free(tmp); return false; }

    // tokenize into array
    char* tokens[256];
    int nt = 0;
    char* tok = strtok(tmp, " \t");
    while (tok && nt < 256) {
        tokens[nt++] = tok;
        tok = strtok(NULL, " \t");
    }
    if (nt < 3) { free(tmp); return false; }

    // last two tokens are qty and price
    char* qtys = tokens[nt - 2];
    char* prices = tokens[nt - 1];

    char* endptr;
    long ql = strtol(qtys, &endptr, 10);
    if (*endptr != '\0') { free(tmp); return false; }
    double pr = strtod(prices, &endptr);
    if (*endptr != '\0') { free(tmp); return false; }

    // reconstruct name from tokens[0..nt-3]
    size_t name_len = 0;
    for (int i = 0; i < nt - 2; ++i) name_len += strlen(tokens[i]) + 1;
    char* name = malloc(name_len + 1);
    if (!name) { free(tmp); return false; }
    name[0] = '\0';
    for (int i = 0; i < nt - 2; ++i) {
        strcat(name, tokens[i]);
        if (i < nt - 3) strcat(name, " ");
    }

    *out_name = name;
    *out_qty = (int)ql;
    *out_price = pr;
    free(tmp);
    return true;
}

/* Load receipts */
Receipt* load_receipts(const char* list_filename) {
    if (!list_filename) return NULL;
    FILE* f = fopen(list_filename, "r");
    if (!f) {
        fprintf(stderr, "Ne mogu otvoriti %s: %s\n", list_filename, strerror(errno));
        return NULL;
    }
    Receipt* head = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (strlen(line) == 0) continue;
        // line is filename of a receipt
        char* fname = line;
        FILE* rf = fopen(fname, "r");
        if (!rf) {
            fprintf(stderr, "Upozorenje: ne mogu otvoriti datoteku racuna '%s' (%s). Preskacem.\n", fname, strerror(errno));
            continue;
        }
        // read first non-empty line as date
        char datebuf[128] = { 0 };
        while (fgets(line, sizeof(line), rf)) {
            trim(line);
            if (strlen(line) == 0) continue;
            strncpy(datebuf, line, 11);
            datebuf[10] = '\0';
            break;
        }
        if (datebuf[0] == '\0') {
            fprintf(stderr, "Upozorenje: datoteka '%s' nema datum. Preskacem.\n", fname);
            fclose(rf);
            continue;
        }
        Receipt* r = receipt_create(datebuf);
        if (!r) { fclose(rf); continue; }

        // read article lines
        while (fgets(line, sizeof(line), rf)) {
            trim(line);
            if (strlen(line) == 0) continue;
            char* aname = NULL;
            int aq = 0;
            double ap = 0.0;
            if (!parse_article_line(line, &aname, &aq, &ap)) {
                fprintf(stderr, "Upozorenje: neispravan red u '%s': %s\n", fname, line);
                continue;
            }
            Article* a = article_create(aname, aq, ap);
            free(aname);
            if (!a) { fprintf(stderr, "Memorijska greska\n"); continue; }
            insert_article_sorted(&r->articles, a);
        }
        fclose(rf);
        insert_receipt_sorted(&head, r);
    }
    fclose(f);
    return head;
}

// Oslobođavamo memoriju
int free_receipts(Receipt* head) {
    Receipt* r = head;
    while (r) {
        Article* a = r->articles;
        while (a) {
            Article* an = a->next;
            free(a->name);
            free(a);
            a = an;
        }
        Receipt* rn = r->next;
        free(r);
        r = rn;
    }
    head->next = NULL;
    return 0;
}

//Printamo sažetak računa
int print_receipts_summary(Receipt* head) {
    Receipt* r = head;
    printf("Sazetak racuna (po datumu):\n");
    while (r) {
        int cnt = 0;
        for (Article* a = r->articles; a; a = a->next) cnt++;
        printf("  %s  - %d artikala\n", r->date, cnt);
        r = r->next;
    }
    return 0;
}

//Detaljni ispis računa
int print_receipts_detailed(Receipt* head) {
    Receipt* r = head;
    printf("Detaljni ispis racuna:\n");
    while (r) {
        printf("Datum: %s\n", r->date);
        for (Article* a = r->articles; a; a = a->next) {
            printf("   %-30s %6d x %8.2f  = %8.2f\n", a->name, a->qty, a->price, a->qty * a->price);
        }
        r = r->next;
        printf("\n");
    }
    return 0;
}

// Upit: za zadano ime artikla (case-insensitive) i datum od-do (YYYY-MM-DD),
int query_article_range(Receipt* head, const char* article_name,
    const char* date_from, const char* date_to,
    int* out_total_qty, double* out_total_spent) {
    if (out_total_qty) *out_total_qty = 0;
    if (out_total_spent) *out_total_spent = 0.0;
    if (!article_name || !date_from || !date_to) return;

    Receipt* r = head;
    while (r) {
        if (cmp_date(r->date, date_from) < 0) { r = r->next; continue; }
        if (cmp_date(r->date, date_to) > 0) break; // list sorted ascending => mozemo prekidati
        for (Article* a = r->articles; a; a = a->next) {
            if (strcasecmp(a->name, article_name) == 0) {
                if (out_total_qty) *out_total_qty += a->qty;
                if (out_total_spent) *out_total_spent += a->qty * a->price;
            }
        }
        r = r->next;
    }
    return 0;
}
