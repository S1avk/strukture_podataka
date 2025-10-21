#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS
#define MAX_CHAR 10

typedef struct osoba* pozicija;
typedef struct osoba {
	char ime[MAX_CHAR];
	char prezime[MAX_CHAR];
	int godina;
	pozicija next;
}osoba;

int dodavanje(pozicija, char* ime, char* prezime, int godina);
int ispis(pozicija);
int dodajKraj(pozicija, char* ime, char* prezime, int godina);
int nadiprezime(pozicija, char* prezime);
int BrisiOsobu(pozicija, char* ime, char* prezime, int godina);

int main() {
	osoba head;
	strcpy(head.ime, ""); strcpy(head.prezime, ""); head.godina = 0; head.next = NULL;

	dodavanje(&head, "Nikola", "Bareta", 04);
	ispis(head.next);
	dodajKraj(&head, "Nikola", "Kurt", 04);
	ispis(head.next);
	nadiprezime(&head, "Bareta");

	BrisiOsobu(&head, "Nikola", "Bareta", 04);
	ispis(head.next);
	return 0;
}

int dodavanje(pozicija p, char* ime, char* prezime, int godina) {
	pozicija q;
	q = (pozicija)malloc(sizeof(osoba));
	if (!q) {
		printf("Greska pri alokaciji");
		return -1;
	}
	strcpy(q->ime, ime);
	strcpy(q->prezime, prezime);
	q->godina = godina;
	q->next = p->next;
	p->next = q;
	return 0;
}

int ispis(pozicija p) {
	if (p == NULL)
		printf("Lista je prazba");
	while (p != NULL) {
		printf("%s %s %d \n", p->ime, p->prezime, p->godina);
		p = p->next;
	}
	printf("\n");
	return 0;
}

int dodajKraj(pozicija p, char* ime, char* prezime, int godina) {
	pozicija q;
	q = (pozicija)malloc(sizeof(osoba));
	if (!q) {
		printf("Greska pri alokaciji");
		return -1;
	}
	strcpy(q->ime, ime);
	strcpy(q->prezime, prezime);
	q->godina = godina;

	while (p->next != NULL) {
		p = p->next;
	}
	q->next = p->next;
	p->next = q;
	return 0;
}
int nadiprezime(pozicija p, char* prezime) {
	p = p->next;
	while (p != NULL) {
		if (strcmp(p->prezime, prezime) == 0) {
			printf("Pronadena je osoba %s \n", p->prezime);
			return 0;
		}
		p = p->next;
	}
	if (p == NULL)
		printf("Osoba nije pronadena\n");
	return 0;
}

int BrisiOsobu(pozicija p, char* ime, char* prezime, int godina) {
	pozicija temp = p->next;
	while (temp != NULL) {
		if (strcmp(temp->ime, ime) == 0 && strcmp(temp->prezime, prezime) == 0 && temp->godina == godina) {
			p->next = temp->next;
			printf("Osoba je izbrisana\n");
			free(temp);
			return 0;
		}
		temp = temp->next;
		p = p->next;
	}
	return 0;
}