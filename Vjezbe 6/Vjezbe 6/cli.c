// cli.c
#define _POSIX_C_SOURCE 200809L
#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int read_line(const char* prompt, char* buf, size_t n) {
    printf("%s", prompt);
    if (!fgets(buf, n, stdin)) {
        buf[0] = '\0';
        return;
    }
    // remove newline
    size_t len = strlen(buf);
    if (len && buf[len - 1] == '\n') buf[len - 1] = '\0';
	return 1;
}

/* Minimal provjera formata datuma (YYYY-MM-DD) */
static int valid_date(const char* d) {
    if (!d) return 0;
    if (strlen(d) != 10) return 0;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) { if (d[i] != '-') return 0; }
        else if (!('0' <= d[i] && d[i] <= '9')) return 0;
    }
    return 1;
}

int run_cli(Receipt* head) {
    char cmd[8];
    char buf[512];
    printf("Pretrazivanje racuna\n");
    while (1) {
        printf("\nMeni:\n");
        printf(" 1) Upit: koliko je potroseno za artikl u razdoblju\n");
        printf(" 2) Ispisi sazetak racuna\n");
        printf(" 3) Ispisi detaljno sve racune\n");
        printf(" 4) Izadi\n");
        read_line("Odabir > ", cmd, sizeof(cmd));
        if (strcmp(cmd, "1") == 0) {
            char art[32], dfrom[16], dto[16];
            read_line("Ime artikla (osjetljivost: NE) > ", art, sizeof(art));
            if (strlen(art) == 0) { printf("Ime ne smije biti prazno.\n"); continue; }
            read_line("Datum od (YYYY-MM-DD) > ", dfrom, sizeof(dfrom));
            read_line("Datum do (YYYY-MM-DD) > ", dto, sizeof(dto));
            if (!valid_date(dfrom) || !valid_date(dto)) {
                printf("Neispravan format datuma. Koristite YYYY-MM-DD.\n");
                continue;
            }
            int total_qty = 0;
            double total_spent = 0.0;
            query_article_range(head, art, dfrom, dto, &total_qty, &total_spent);
            printf("\nRezultat za \"%s\" od %s do %s:\n", art, dfrom, dto);
            printf("  Ukupna kolicina: %d\n", total_qty);
            printf("  Ukupno potroseno: %.2f\n", total_spent);
        }
        else if (strcmp(cmd, "2") == 0) {
            print_receipts_summary(head);
        }
        else if (strcmp(cmd, "3") == 0) {
            print_receipts_detailed(head);
        }
        else if (strcmp(cmd, "4") == 0 || strcmp(cmd, "q") == 0) {
            printf("Dovidenja.\n");
            break;
        }
        else {
            printf("Nepoznata opcija.\n");
        }
    }
    return 0;
}
