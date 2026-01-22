#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define max_name 100

typedef struct Dir {
    char name[max_name];
    struct Dir* parent;
    struct Dir* first_child;
    struct Dir* next_sibling;
} Dir;

Dir* create_dir(const char* name, Dir* parent);
bool add_child_dir(Dir* current, const char* name);
Dir* find_child(Dir* current, const char* name);
Dir* change_dir(Dir* current, const char* name);
Dir* go_parent(Dir* current);
void list_dir(Dir* current);
void menu_loop(Dir* root);
void free_tree(Dir* node);



int main() {
    Dir* root = create_dir("root", NULL);
    if (!root) {
        return -1;
    }
    menu_loop(root);
    free_tree(root);
	return 0;
}

Dir* create_dir(const char* name, Dir* parent) {
    Dir* new_dir = (Dir*)malloc(sizeof(Dir));
    if (!new_dir) {
        printf("Failed to allocate memory for new directory");
        return NULL;
    }

    strncpy(new_dir->name, name, max_name);
    new_dir->parent = parent;
    new_dir->first_child = NULL;
    new_dir->next_sibling = NULL;
    return new_dir;
}

bool add_child_dir(Dir* current, const char* name) {
    if (!current) {
        printf("Current directory is NULL\n");
        return false;
    }

    Dir* new_dir = create_dir(name, current);
    if (!new_dir) {
        return false;
    }

    if (!current->first_child) {
        current->first_child = new_dir; 
    } else {
        Dir* temp = current->first_child;
        while (temp->next_sibling) {
            temp = temp->next_sibling;
        }
        temp->next_sibling = new_dir;
    }

    return true;
}

Dir* find_child(Dir* current, const char* name) {
    if (!current) {
        return NULL;
    }
    Dir* temp = current->first_child;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next_sibling;
    }
    return NULL;
}

Dir* change_dir(Dir* current, const char* name) {
    if (strcmp(name, "..") == 0) {
        return current->parent ? current->parent : current;
    }
    Dir* child = find_child(current, name);
    if (child) {
        return child;
    } else {
        printf("Directory '%s' not found\n", name);
        return current;
    }
}

Dir* go_parent(Dir* current) {
    if (current->parent) {
        return current->parent;
    } else {
        printf("Already at root directory\n");
        return current;
    }
}

void list_dir(Dir* current) {
    if (!current) {
        printf("Current directory is NULL\n");
        return;
    }
    Dir* temp = current->first_child;
    if (!temp) {
        printf("No subdirectories\n");
        return;
    }
    while (temp) {
        printf("%s\n", temp->name);
        temp = temp->next_sibling;
    }
}

void menu_loop(Dir* root) {
    Dir* current = root;
    char command[max_name];
    char dir_name[max_name];
    while (1) {
        printf("Current directory: %s\n", current->name);
        printf("Enter command (md, cd dir, cd.., dir, exit): ");
        scanf("%s", command);
        if (strcmp(command, "md") == 0) {
            printf("Enter directory name: ");
            scanf("%s", dir_name);
            if (add_child_dir(current, dir_name)) {
                printf("Directory '%s' created\n", dir_name);
            }
        } else if (strcmp(command, "cddir") == 0) {
            printf("Enter directory name: ");
            scanf("%s", dir_name);
            current = change_dir(current, dir_name);
        } else if(strcmp(command, "cd..") == 0) {
			current = go_parent(current);
        } else if (strcmp(command, "dir") == 0) {
            list_dir(current);
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command\n");
        }
    }
}

void free_tree(Dir* node) {
    if (!node) return;
    Dir* child = node->first_child;
    while (child) {
        Dir* next_sibling = child->next_sibling;
        free_tree(child);
        child = next_sibling;
    }
    free(node);
}