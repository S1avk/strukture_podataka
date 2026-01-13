#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct racun* Position;
typedef struct racun {
    int day;
    int month;
    int year;
    Position next;
    Position head;
} racun;

typedef struct artikl* Position;
typedef struct artikl {
    char* naziv;
    int količina;
    double cijena;
    Position next;
} artikl;

int main() {
    FILE* fp = fopen("racuni.txt", "r");
    if (!fp) {
        printf("Greška pri otvaranju datoteke!\n");
        return 1;
    }

    racun head = { .day = 0, .month = 0, .year = 0, .next = NULL };
    artikl head = { .naziv = "", .količina = 0, .cijena = 0, .next = NULL};

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (process_line(buffer, &head) != 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
}

int create_bill(Position head) {
    Position new_bill = (Position)malloc(sizeof(racun));
    if (!new_bill) {
        return 0;
    }
    new_bill->next = head->next;
    head->next = new_bill;
    return 0;
}

int create_article(Position head) {
    Position new_article = (Position)malloc(sizeof(artikl));
    if (!new_article) {
        return 0;
    }
    new_article->next = head->next;
    head->next = new_article;
    return 0;
}

int process_line_bill(char* line, Position head) {
    char* p = line;
	int counter = 0;
    while (*p != '\0' && counter < 3) {

        // višeznamenkasti broj
        if (isdigit(*p) || (isdigit(*(p + 1)))) {
            int num = strtoi(p, &p);

            
        }
        else {
            counter++;
        }
    }

    return 0;
}

int process_line_article(char* line, Position head) {
    char* p = line;
    while (*p != '\0') {

        // višeznamenkasti broj
        if (isdigit(*p) || (isdigit(*(p + 1)))) {
            int num = strtoi(p, &p);

            if (!Push(head, num)) {
                printf("Greška pri alokaciji memorije.\n");
                return 1;
            }
            continue;
        }
    }

    return 0;
}