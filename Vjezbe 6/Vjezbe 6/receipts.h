// receipts.h
#ifndef RECEIPTS_H
#define RECEIPTS_H

#include <stdio.h>

typedef struct Article {
    char* name;
    int qty;
    double price;
    struct Article* next;
} Article;

typedef struct Receipt {
    char date[11]; // "YYYY-MM-DD"
    Article* articles;
    struct Receipt* next;
} Receipt;

/* Ucitava sve racune navedenih u datoteci list_filename (npr. "racuni.txt").
   Vraca pokazivac na glavu liste (sortirano po datumu uzlazno).
   Ime artikla moze sadrzavati razmake; zadnja dva tokena retka su kolicina i cijena. */
Receipt* load_receipts(const char* list_filename);

/* Oslobadanje memorije */
int free_receipts(Receipt* head);

/* Ispis sazetka racuna (datum i broj artikala) */
int print_receipts_summary(Receipt* head);

/* Ispis detaljno svi racuni i njihovi artikli */
int print_receipts_detailed(Receipt* head);

/* Upit: za zadano ime artikla (case-insensitive) i datum od-do (YYYY-MM-DD),
   vraca ukupnu kolicinu i ukupno potroseni novac. */
int query_article_range(Receipt* head, const char* article_name,
    const char* date_from, const char* date_to,
    int* out_total_qty, double* out_total_spent);

#endif // RECEIPTS_H
#pragma once
