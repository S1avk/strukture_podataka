// main.c
#include "receipts.h"
#include "cli.h"
#include <stdio.h>

int main() {
    const char* list_file = "racuni.txt";
    Receipt* head = load_receipts(list_file);
    if (!head) {
        fprintf(stderr, "Nema ucitanih racuna (provjerite '%s').\n", list_file);
        // i dalje omogucimo CLI s praznom listom (mozda samo ispis poruka)
    }
    run_cli(head);
    free_receipts(head);
    return 0;
}
