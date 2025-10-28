#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 10

typedef struct osoba* pozicija;
typedef struct osoba {
    char ime[MAX_CHAR];
    char prezime[MAX_CHAR];
    int godina;
    pozicija next;
} osoba;

int dodavanje(pozicija, char*, char*, int);
int ispis(pozicija);
int kraj(pozicija, char*, char*, int);
int nadiprezime(pozicija, char*);
int brisanje(pozicija, char*, char*, int);
int dodajIza(pozicija, char*, char*, int, char*);
int dodajIspred(pozicija, char*, char*, int, char*);
int sortiraj(pozicija);
int upisUDatoteku(pozicija, char*);
int citajIzDatoteke(pozicija, char*);

int main() {
    osoba head;
    strcpy(head.ime, ""); strcpy(head.prezime, ""); head.godina = 0; head.next = NULL;

    dodavanje(&head, "Slavko", "Kozina", 2004);
    kraj(&head, "Ante", "Pirija", 2004);
    dodavanje(&head, "Nikola", "Bareta", 2004);

    printf("Pocetna lista:\n");
    ispis(head.next);

    dodajIza(&head, "Marin", "Devalle", 2004, "Kozina");
    dodajIspred(&head, "Bruno", "Kosta", 2005, "Pirija");

    printf("\nLista nakon dodavanja:\n");
    ispis(head.next);

    sortiraj(&head);
    printf("\nSortirana lista:\n");
    ispis(head.next);

    upisUDatoteku(head.next, "osobe.txt");

    osoba head2;
    strcpy(head2.ime, ""); strcpy(head2.prezime, ""); head2.godina = 0; head2.next = NULL;
    citajIzDatoteke(&head2, "osobe.txt");

    printf("\nLista ucitana iz datoteke:\n");
    ispis(head2.next);

    return 0;
}

// 2A. Dodavanje novog elementa na početak liste
int dodavanje(pozicija p, char* ime, char* prezime, int godina) {
    pozicija q = (pozicija)malloc(sizeof(osoba));
    if (!q) return -1;
    strcpy(q->ime, ime);
    strcpy(q->prezime, prezime);
    q->godina = godina;
    q->next = p->next;
    p->next = q;
    return 0;
}

// 2B. Ispis liste
int ispis(pozicija p) {
    if (p == NULL) {
        printf("Lista je prazna\n");
        return 0;
    }
    while (p != NULL) {
        printf("%s %s %d\n", p->ime, p->prezime, p->godina);
        p = p->next;
    }
    return 0;
}

// 2C. Dodavanje novog elementa na kraj liste
int kraj(pozicija p, char* ime, char* prezime, int godina) {
    while (p->next != NULL) p = p->next;
    dodavanje(p, ime, prezime, godina);
    return 0;
}

// 2D. Pronalaženje elementa iz liste
int nadiprezime(pozicija p, char* prezime) {
    p = p->next;
    while (p != NULL) {
        if (strcmp(p->prezime, prezime) == 0) {
            printf("Pronadena osoba: %s %s\n", p->ime, p->prezime);
            return 0;
        }
        p = p->next;
    }
    printf("Osoba nije pronadena\n");
    return 0;
}

// 2E. Brisanje elementa iz liste
int brisanje(pozicija p, char* ime, char* prezime, int godina) {
    pozicija temp;
    while (p->next != NULL) {
        temp = p->next;
        if (strcmp(temp->ime, ime) == 0 && strcmp(temp->prezime, prezime) == 0 && temp->godina == godina) {
            p->next = temp->next;
            free(temp);
            return 0;
        }
        p = p->next;
    }
    return 0;
}

// 3A. Dodavanje iza određenog elementa
int dodajIza(pozicija p, char* ime, char* prezime, int godina, char* targetPrezime) {
    pozicija q = p->next;
    while (q != NULL && strcmp(q->prezime, targetPrezime) != 0)
        q = q->next;
    if (q == NULL) return -1;
    dodavanje(q, ime, prezime, godina);
    return 0;
}

// 3B. Dodavanje ispred određenog elementa
int dodajIspred(pozicija p, char* ime, char* prezime, int godina, char* targetPrezime) {
    while (p->next != NULL && strcmp(p->next->prezime, targetPrezime) != 0)
        p = p->next;
    if (p->next == NULL) return -1;
    dodavanje(p, ime, prezime, godina);
    return 0;
}

// 3C. Sortiranje po prezimenima
int sortiraj(pozicija head) {
    pozicija i, j, prev, temp;
    int swapped;
    do {
        swapped = 0;
        prev = head;
        i = head->next;
        while (i != NULL && i->next != NULL) {
            j = i->next;
            if (strcmp(i->prezime, j->prezime) > 0) {
                i->next = j->next;
                j->next = i;
                prev->next = j;
                swapped = 1;
            }
            prev = prev->next;
            i = prev->next;
        }
    } while (swapped);
    return 0;
}

// 3D. Upis u datoteku
int upisUDatoteku(pozicija p, char* imeDatoteke) {
    FILE* fp = fopen(imeDatoteke, "w");
    if (!fp) return -1;
    while (p != NULL) {
        fprintf(fp, "%s %s %d\n", p->ime, p->prezime, p->godina);
        p = p->next;
    }
    fclose(fp);
    return 0;
}

// 3E. Čitanje iz datoteke
int citajIzDatoteke(pozicija head, char* imeDatoteke) {
    FILE* fp = fopen(imeDatoteke, "r");
    if (!fp) return -1;
    char ime[MAX_CHAR], prezime[MAX_CHAR];
    int godina;
    while (fscanf(fp, "%s %s %d", ime, prezime, &godina) == 3) {
        kraj(head, ime, prezime, godina);
    }
    fclose(fp);
    return 0;
}
