#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct postfix* Position;
typedef struct postfix {
    double number;
    Position next;
} postfix;

int Push(Position head, double num);
int Pop(Position head, double* a, double* b);
void FreeList(Position head);
int process_line(char* line, Position head);

int main() {
    FILE* fp = fopen("postfix.txt", "r");
    if (!fp) {
        printf("Greška pri otvaranju datoteke!\n");
        return 1;
    }

    postfix head = { .number = 0, .next = NULL };

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (process_line(buffer, &head) != 0) {
            FreeList(&head);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);

    if (!head.next || head.next->next != NULL) {
        printf("Greška unutar jednadžbe.\n");
        FreeList(&head);
        return 1;
    }

    printf("Rezultat = %.2f\n", head.next->number);

    FreeList(&head);
    return 0;
}

// isčitavanje jednadžbe iz linije
int process_line(char* line, Position head) {
    char* p = line;

    while (*p != '\0') {

        // preskoči razmake
        if (isspace(*p)) {
            p++;
            continue;
        }

        // višeznamenkasti broj
        if (isdigit(*p) || (*p == '-' && isdigit(*(p + 1)))) {
            double num = strtod(p, &p);

            if (!Push(head, num)) {
                printf("Greška pri alokaciji memorije.\n");
                return 1;
            }
            continue;
        }

        // operatori
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
            char op = *p;
            p++;

            double a, b;
            if (!Pop(head, &a, &b)) {
                printf("Nedovoljno operatora.\n");
                return 1;
            }

            double result = 0;

            switch (op) {
            case '+': result = b + a; break;
            case '-': result = b - a; break;
            case '*': result = b * a; break;
            case '/':
                if (a == 0) {
                    printf("Djeljenje s nulom!\n");
                    return 1;
                }
                result = b / a;
                break;
            }

            if (!Push(head, result)) {
                printf("Greška pri alokaciji memorije.\n");
                return 1;
            }

            continue;
        }

        // nepoznat znak
        return 1;
    }

    return 0;
}

int Push(Position head, double num) {
    Position newEl = (Position)malloc(sizeof(postfix));
    if (!newEl) return 0;

    newEl->number = num;
    newEl->next = head->next;
    head->next = newEl;

    return 1;
}

int Pop(Position head, double* a, double* b) {
    Position q = head;

    Position first = q->next;
    if (!first) return 0;

    Position second = first->next;
    if (!second) return 0;

    *a = first->number;
    *b = second->number;

    q->next = second->next;

    first->next = NULL;
    second->next = NULL;

    free(first);
    free(second);

    return 1;
}

void FreeList(Position head) {
    Position temp = head;
    while (temp) {
        Position next = temp->next;
		temp->next = NULL;
        free(temp);
        temp = next;
    }
}
