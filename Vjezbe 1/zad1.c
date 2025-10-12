#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define max 100
#define buffer 256

typedef struct {
    char ime[max];
    char prezime[max];
    int bodovi;
    double relativni;
} stud;

int main(int argc, char* argv[]) {
    FILE* fp;
    char line[buffer];
    int count = 0;
    stud* niz = NULL;
    int i;

 

    fp = fopen("studenti.txt", "r");
    if (!fp) {
        printf("Greska pri otvaranju datoteke.\n");
        return 1;
    }

    //brojanje linija
    while (fgets(line, sizeof(line), fp) != NULL) {
        char ime[max], prezime[max];
        int bod;

        if (sscanf(line, "%s %s %d", ime, prezime, &bod) == 3) {
            count++;
        }
    }

	//provjera prebrojavanja
    if (count == 0) {
        printf("Nema valjanih zapisa u datoteci.\n");
        fclose(fp);
        return 0;
    }

    //alociranje memorije za niz
    niz = (stud*)malloc(sizeof(stud) * count);

    //čitanje podataka
    rewind(fp);
    i = 0;
    while (fgets(line, sizeof(line), fp) != NULL && i < count) {
        char ime[max], prezime[max];
        int bod;
        if (sscanf(line, "%s %s %d", ime, prezime, &bod) == 3) {
            strncpy(niz[i].ime, ime, max - 1);
            niz[i].ime[max - 1] = '\0';
            strncpy(niz[i].prezime, prezime, max - 1);
            niz[i].prezime[max - 1] = '\0';
            niz[i].bodovi = bod;
            niz[i].relativni = 0.0; /* inicijalno */
            i++;
        }
    }

    fclose(fp);

	//traženje maksimalnog broja bodova
    int max_bodova = niz[0].bodovi;
    for (int j = 1; j < count; ++j) {
        if (niz[j].bodovi > max_bodova) max_bodova = niz[j].bodovi;
    }

	//ispis rezultata
    printf("%s \t %s \t %s \t %s\n", "Ime", "Prezime", "Bodovi", "Relativni bodovi");
    printf("---------------------------------------------------------\n");
    for (int j = 0; j < count; ++j) {
        if (max_bodova > 0) {
            niz[j].relativni = ((double)niz[j].bodovi / (double)max_bodova) * 100.0;
        }
        else {
            niz[j].relativni = 0.0;
        }
        printf("%s \t %s \t %d \t\t %.2f\n",
            niz[j].ime, niz[j].prezime, niz[j].bodovi, niz[j].relativni);
    }

    free(niz);
    return 0;
}
