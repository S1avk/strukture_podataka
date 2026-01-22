#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char* strdup_safe(const char* s) {
    if (!s) return NULL;
    char* d = malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

static void trim(char* s) {
    if (!s) return;
    char* p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    char* end = s + strlen(s) - 1;
    while (end >= s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
}

typedef struct CityTree {
    char* name;
    int pop;
    struct CityTree* left;
    struct CityTree* right;
} CityTree;

typedef struct CityList {
    char* name;
    int pop;
    struct CityList* next;
} CityList;

typedef struct CountryList {
    char* name;
    CityTree* cities;
    struct CountryList* next;
} CountryList;

static int city_cmp_by_pop_then_name(const char* aname, int apop, const char* bname, int bpop) {
    if (apop != bpop) return (bpop - apop);
    return strcmp(aname, bname);
}

static CityTree* citytree_create(const char* name, int pop) {
    CityTree* n = malloc(sizeof(CityTree));
    if (!n) return NULL;
    n->name = strdup_safe(name);
    n->pop = pop;
    n->left = n->right = NULL;
    return n;
}

static CityTree* insert_city_tree(CityTree* root, const char* name, int pop) {
    if (!root) return citytree_create(name, pop);
    int cmp = city_cmp_by_pop_then_name(name, pop, root->name, root->pop);
    if (cmp < 0) {
        root->left = insert_city_tree(root->left, name, pop);
    }
    else {
        root->right = insert_city_tree(root->right, name, pop);
    }
    return root;
}

static void print_city_tree_inorder(const CityTree* root) {
    if (!root) return;
    print_city_tree_inorder(root->left);
    printf("      %-30s : %d\n", root->name, root->pop);
    print_city_tree_inorder(root->right);
}

static void free_city_tree(CityTree* root) {
    if (!root) return;
    free_city_tree(root->left);
    free_city_tree(root->right);
    free(root->name);
    free(root);
}

static CityList* citylist_create(const char* name, int pop) {
    CityList* n = malloc(sizeof(CityList));
    if (!n) return NULL;
    n->name = strdup_safe(name);
    n->pop = pop;
    n->next = NULL;
    return n;
}

static int insert_city_list_sorted(CityList** head, const char* name, int pop) {
    if (!head) return 0;
    CityList* node = citylist_create(name, pop);
    if (!node) return 0;
    if (!*head) {
        *head = node;
        return 1;
    }

    int cmp = city_cmp_by_pop_then_name(name, pop, (*head)->name, (*head)->pop);
    if (cmp < 0) {
        node->next = *head;
        *head = node;
        return 1;
    }
    CityList* cur = *head;
    while (cur->next && city_cmp_by_pop_then_name(name, pop, cur->next->name, cur->next->pop) >= 0) {
        cur = cur->next;
    }
    node->next = cur->next;
    cur->next = node;
    return 1;
}

static void print_city_list(const CityList* head) {
    const CityList* cur = head;
    while (cur) {
        printf("      %-30s : %d\n", cur->name, cur->pop);
        cur = cur->next;
    }
}

static void free_city_list(CityList* head) {
    CityList* cur = head;
    while (cur) {
        CityList* n = cur->next;
        free(cur->name);
        free(cur);
        cur = n;
    }
}


static CountryList* countrylist_create(const char* name) {
    CountryList* n = malloc(sizeof(CountryList));
    if (!n) return NULL;
    n->name = strdup_safe(name);
    n->cities = NULL;
    n->next = NULL;
    return n;
}

static int insert_country_list_sorted(CountryList** head, CountryList* node) {
    if (!head || !node) return 0;
    if (!*head || strcmp(node->name, (*head)->name) < 0) {
        node->next = *head;
        *head = node;
        return 1;
    }
    CountryList* cur = *head;
    while (cur->next && strcmp(cur->next->name, node->name) <= 0) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return 1;
}


static int parse_country_line(const char* line, char** out_country, char** out_filename) {
    if (!line) return 0;
    char tmp[1024];
    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    trim(tmp);
    if (strlen(tmp) == 0) return 0;
    char* tok = strtok(tmp, " \t");
    if (!tok) return 0;
    *out_country = strdup_safe(tok);
    tok = strtok(NULL, " \t");
    if (!tok) {
        free(*out_country);
        *out_country = NULL;
        return 0;
    }
    *out_filename = strdup_safe(tok);
    return 1;
}

static int parse_city_line(const char* line, char** out_name, int* out_pop) {
    if (!line) return 0;
    char tmp[1024];
    strncpy(tmp, line, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    trim(tmp);
    if (strlen(tmp) == 0) return 0;

    char* last_comma = strrchr(tmp, ',');
    if (!last_comma) return 0;
    *last_comma = '\0';
    char* name = tmp;
    char* popstr = last_comma + 1;
    trim(name);
    trim(popstr);
    if (strlen(name) == 0 || strlen(popstr) == 0) return 0;
    char* endptr;
    long p = strtol(popstr, &endptr, 10);
    if (*endptr != '\0') return 0;
    *out_name = strdup_safe(name);
    *out_pop = (int)p;
    return 1;
}


#define HASH_SIZE 11

static int country_hash(const char* name, int table_size) {
    if (!name || table_size <= 0) return 0;
    int sum = 0;
    for (int i = 0; i < 5 && name[i] != '\0'; ++i) sum += (unsigned char)name[i];
    return sum % table_size;
}


static int load_data(const char* list_filename, CountryList* table[], int table_size) {
    if (!list_filename || !table || table_size <= 0) return 0;
    FILE* f = fopen(list_filename, "r");
    if (!f) {
        fprintf(stderr, "Ne mogu otvoriti %s\n", list_filename);
        return 0;
    }
    char line[1024];

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (strlen(line) == 0) continue;
        char* country = NULL;
        char* fname = NULL;
        if (!parse_country_line(line, &country, &fname)) {
            fprintf(stderr, "Neispravan red u %s: %s\n", list_filename, line);
            continue;
        }
        CountryList* clnode = countrylist_create(country);
        free(country);
        if (!clnode) {
            fprintf(stderr, "Memorijska greska\n");
            free(fname);
            continue;
        }

        FILE* gf = fopen(fname, "r");
        if (!gf) {
            fprintf(stderr, "Ne mogu otvoriti '%s'.\n", fname);
            free(fname);
            
            int idx = country_hash(clnode->name, table_size);
            insert_country_list_sorted(&table[idx], clnode);
            continue;
        }
        free(fname);

        while (fgets(line, sizeof(line), gf)) {
            trim(line);
            if (strlen(line) == 0) continue;
            char* cityname = NULL;
            int pop = 0;
            if (!parse_city_line(line, &cityname, &pop)) {
                fprintf(stderr, "  Preskacem neispravan red u datoteci gradova: %s\n", line);
                continue;
            }

            clnode->cities = insert_city_tree(clnode->cities, cityname, pop);
            free(cityname);
        }
        fclose(gf);

        int idx = country_hash(clnode->name, table_size);
        insert_country_list_sorted(&table[idx], clnode);
    }

    fclose(f);
    return 1;
}

static void print_hash_table(CountryList* table[], int table_size) {
    printf("=== Hash table of countries (%d buckets) - each node has a city tree ===\n", table_size);
    for (int i = 0; i < table_size; ++i) {
        printf("Bucket %2d:\n", i);
        if (!table[i]) {
            printf("  (empty)\n\n");
            continue;
        }
        CountryList* cur = table[i];
        while (cur) {
            printf("  %s:\n", cur->name);
            print_city_tree_inorder(cur->cities);
            cur = cur->next;
            printf("\n");
        }
    }
}

static void free_hash_table(CountryList* table[], int table_size) {
    for (int i = 0; i < table_size; ++i) {
        CountryList* cur = table[i];
        while (cur) {
            CountryList* n = cur->next;
            free(cur->name);
            free_city_tree(cur->cities);
            free(cur);
            cur = n;
        }
        table[i] = NULL;
    }
}

static CountryList* find_country_in_hash_table(CountryList* table[], int table_size, const char* name) {
    if (!table || table_size <= 0 || !name) return NULL;
    int idx = country_hash(name, table_size);
    for (CountryList* cur = table[idx]; cur; cur = cur->next) {
        if (strcmp(cur->name, name) == 0) return cur;
    }
    return NULL;
}

static void print_cities_tree_above(const CityTree* root, int threshold) {
    if (!root) return;

    print_cities_tree_above(root->left, threshold);
    if (root->pop > threshold) printf("      %-30s : %d\n", root->name, root->pop);
    print_cities_tree_above(root->right, threshold);
}

static void print_cities_list_above(const CityList* head, int threshold) {
    const CityList* cur = head;
    while (cur) {
        if (cur->pop > threshold) printf("      %-30s : %d\n", cur->name, cur->pop);
        cur = cur->next;
    }
}

int main(void) {
    CountryList* hash_table[HASH_SIZE] = { 0 };

    if (!load_data("drzave.txt", hash_table, HASH_SIZE)) {
        fprintf(stderr, "Greska pri ucitavanju podataka.\n");
        return 1;
    }

    print_hash_table(hash_table, HASH_SIZE);

    char buf[256];
    while (1) {
        printf("Unesite naziv drzave za pretragu (ili 'q' za izlaz): ");
        if (!fgets(buf, sizeof(buf), stdin)) break;
        trim(buf);
        if (strcmp(buf, "q") == 0 || strcmp(buf, "Q") == 0) break;
        if (strlen(buf) == 0) continue;
        char country_query[256];
        strncpy(country_query, buf, sizeof(country_query) - 1);
        country_query[sizeof(country_query) - 1] = '\0';

        printf("Unesite prag broja stanovnika (ispis gradova sa pop > prag): ");
        if (!fgets(buf, sizeof(buf), stdin)) break;
        trim(buf);
        if (strlen(buf) == 0) continue;
        char* endptr;
        long thresh = strtol(buf, &endptr, 10);
        if (*endptr != '\0') {
            printf("Neispravan broj. Pokusajte ponovo.\n");
            continue;
        }

        printf("\nRezultati (pretraga u hash tablici -> stablo gradova):\n");

        CountryList* cl = find_country_in_hash_table(hash_table, HASH_SIZE, country_query);
        if (cl) {
            printf("  (iz hash tablice -> stablo gradova)\n");
            print_cities_tree_above(cl->cities, (int)thresh);
        }
        else {
            printf("  Drzava '%s' nije pronadena u hash tablici.\n", country_query);
        }

        printf("\n");
    }

    free_hash_table(hash_table, HASH_SIZE);

    printf("Kraj programa.\n");
    return 0;
}