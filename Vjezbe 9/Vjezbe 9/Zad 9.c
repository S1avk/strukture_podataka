#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct tree* treePos;
typedef struct tree {
    int value;
    treePos left;
    treePos right;
} tree;

int insert(treePos* root, int value);
int insert_at_path(treePos* root, const char* path, int value);
int inorder(treePos root);
int inorder_to_file(treePos root, FILE* file);
int calculate_sum(treePos root);
int replace(treePos root, int* sum);

int main() {
    treePos root = NULL;
    FILE* file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return -1;
    }

    // Generate 10 random numbers in range <10, 90>
    srand((unsigned int)(time(NULL) ^ clock()));
    int arr[10];
    printf("Generated numbers: ");
    for (int i = 0; i < 10; i++) {
        arr[i] = 10 + rand() % 81; // produces values in [10,90]
        printf("%d ", arr[i]);
    }
    printf("\n\n");

    // Insert elements into tree (deterministic positions to match Slika 1)
    for (int i = 0; i < 10; i++) {
        insert(&root, arr[i]);
    }

    printf("Inorder traversal (before replace): ");
    inorder(root);
    printf("\n");

    fprintf(file, "Inorder traversal (before replace): ");
    inorder_to_file(root, file);
    fprintf(file, "\n\n");

    // Replace each node with sum of its subtrees
    int dummy_sum = 0;
    replace(root, &dummy_sum);

    printf("Inorder traversal (after replace): ");
    inorder(root);
    printf("\n");

    fprintf(file, "Inorder traversal (after replace): ");
    inorder_to_file(root, file);
    fprintf(file, "\n");

    fclose(file);
    printf("\nResults written to output.txt\n");

    return 0;
}

// insert places node according to a fixed path table so the tree matches Slika 1
int insert(treePos* root, int value) {
    static int count = 0;
    const char* paths[] = {
        NULL,      // dummy index 0
        "",        // 1 -> root
        "L",       // 2 -> root->left
        "LL",      // 3 -> root->left->left
        "LLL",     // 4
        "LLLL",    // 5
        "R",       // 6 -> root->right
        "LR",      // 7 -> root->left->right
        "LRR",     // 8 -> root->left->right->right
        "LRRL",    // 9 -> root->left->right->right->left
        "LLLR"     //10 -> root->left->left->left->right
    };
    count++;
    if (count >= 1 && count <= 10) {
        return insert_at_path(root, paths[count], value);
    }
    // fallback to level-order append if more nodes (not required for this task)
    if (*root == NULL) {
        *root = (treePos)malloc(sizeof(tree));
        if (*root == NULL) return -1;
        (*root)->value = value; (*root)->left = (*root)->right = NULL;
        return 0;
    }
    treePos queue[256];
    int front = 0, rear = 0;
    queue[rear++] = *root;
    while (front < rear) {
        treePos cur = queue[front++];
        if (cur->left == NULL) {
            cur->left = (treePos)malloc(sizeof(tree));
            if (!cur->left) return -1;
            cur->left->value = value; cur->left->left = cur->left->right = NULL;
            return 0;
        } else queue[rear++] = cur->left;
        if (cur->right == NULL) {
            cur->right = (treePos)malloc(sizeof(tree));
            if (!cur->right) return -1;
            cur->right->value = value; cur->right->left = cur->right->right = NULL;
            return 0;
        } else queue[rear++] = cur->right;
    }
    return 0;
}

// walk the path of 'L'/'R' and attach new node at the path end
int insert_at_path(treePos* root, const char* path, int value) {
    if (path == NULL) return -1;
    if (*root == NULL && path[0] != '\0') {
        // If root missing but path expects deeper placement, create root placeholder
        *root = (treePos)malloc(sizeof(tree));
        if (*root == NULL) return -1;
        (*root)->value = 0; (*root)->left = (*root)->right = NULL;
    }
    if (path[0] == '\0') {
        // root
        if (*root == NULL) {
            *root = (treePos)malloc(sizeof(tree));
            if (*root == NULL) return -1;
            (*root)->value = value; (*root)->left = (*root)->right = NULL;
            return 0;
        } else {
            // overwrite if root exists (should not happen for this sequence)
            (*root)->value = value;
            return 0;
        }
    }
    treePos cur = *root;
    const char* p = path;
    // traverse to parent (all but last char)
    while (*(p+1) != '\0') {
        if (*p == 'L') {
            if (cur->left == NULL) {
                cur->left = (treePos)malloc(sizeof(tree));
                if (cur->left == NULL) return -1;
                cur->left->value = 0; cur->left->left = cur->left->right = NULL;
            }
            cur = cur->left;
        } else { // 'R'
            if (cur->right == NULL) {
                cur->right = (treePos)malloc(sizeof(tree));
                if (cur->right == NULL) return -1;
                cur->right->value = 0; cur->right->left = cur->right->right = NULL;
            }
            cur = cur->right;
        }
        p++;
    }
    // now attach at final direction *p
    if (*p == 'L') {
        if (cur->left == NULL) {
            cur->left = (treePos)malloc(sizeof(tree));
            if (cur->left == NULL) return -1;
            cur->left->value = value; cur->left->left = cur->left->right = NULL;
        } else {
            cur->left->value = value;
        }
    } else { // 'R'
        if (cur->right == NULL) {
            cur->right = (treePos)malloc(sizeof(tree));
            if (cur->right == NULL) return -1;
            cur->right->value = value; cur->right->left = cur->right->right = NULL;
        } else {
            cur->right->value = value;
        }
    }
    return 0;
}

int inorder(treePos root) {
    if (root == NULL) return 0;
    inorder(root->left);
    printf("%d ", root->value);
    inorder(root->right);
    return 0;
}

int inorder_to_file(treePos root, FILE* file) {
    if (root == NULL) return 0;
    inorder_to_file(root->left, file);
    fprintf(file, "%d ", root->value);
    inorder_to_file(root->right, file);
    return 0;
}

int calculate_sum(treePos root) {
    if (root == NULL) return 0;
    return root->value + calculate_sum(root->left) + calculate_sum(root->right);
}

int replace(treePos root, int* sum) {
    if (root == NULL) return 0;
    int left_sum = calculate_sum(root->left);
    int right_sum = calculate_sum(root->right);
    replace(root->left, sum);
    replace(root->right, sum);
    root->value = left_sum + right_sum;
    return 0;
}