#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct tree* treePos;
typedef struct tree {
	int value;
	treePos left;
	treePos right;
} tree;

int insert(treePos* root, int value);
int delete(treePos* root, int value);
int inorder(treePos root);
int preorder(treePos root);
int postorder(treePos root);
int levelOrder(treePos root);

int main() {
	treePos root = NULL;
	insert(&root, 50);
	insert(&root, 30);
	insert(&root, 70);
	insert(&root, 20);
	insert(&root, 40);
	insert(&root, 60);
	insert(&root, 80);
	printf("Inorder traversal: ");
	inorder(root);
	printf("\n");
	printf("Preorder traversal: ");
	preorder(root);
	printf("\n");
	printf("Postorder traversal: ");
	postorder(root);
	printf("\n");
	printf("Level-order traversal: ");
	levelOrder(root);
	printf("\n");
	delete(root, 20);
	printf("Inorder traversal after deleting 20: ");
	inorder(root);
	return 0;
}

int insert(treePos* root, int value) {
	if (*root == NULL) {
		*root = (treePos)malloc(sizeof(tree));
		if (*root == NULL) {
			printf("Memory allocation failed!\n");
			return -1;
		}
		(*root)->value = value;
		(*root)->left = NULL;
		(*root)->right = NULL;
		return 0;
	}
	if (value < (*root)->value) {
		return insert(&(*root)->left, value);
	} else if (value > (*root)->value) {
		return insert(&(*root)->right, value);
	} else {
			printf("Value %d already exists in the tree.\n", value);
			return -1;
		}
}

int delete(treePos* root, int value) {
	if (*root == NULL) {
		printf("Value %d not found in the tree.\n", value);
		return -1;
	}
	if (value < (*root)->value) {
		return delete(&(*root)->left, value);
	} else if (value > (*root)->value) {
		return delete(&(*root)->right, value);
	} else {
		if ((*root)->left == NULL && (*root)->right == NULL) {
			free(*root);
			*root = NULL;
		} else if ((*root)->left == NULL) {
			treePos temp = *root;
			*root = (*root)->right;
			free(temp);
		} else if ((*root)->right == NULL) {
			treePos temp = *root;
			*root = (*root)->left;
			free(temp);
		} else {
			treePos temp = (*root)->right;
			while (temp->left != NULL) {
				temp = temp->left;
			}
			(*root)->value = temp->value;
			delete(&(*root)->right, temp->value);
		}
		return 0;
	}
}

int inorder(treePos root) {
	if (root == NULL) {
		return 0;
	}
	inorder(root->left);
	printf("%d ", root->value);
	inorder(root->right);
	return 0;
}

int preorder(treePos root) {
	if (root == NULL) {
		return 0;
	}
	printf("%d ", root->value);
	preorder(root->left);
	preorder(root->right);
	return 0;
}

int postorder(treePos root) {
	if (root == NULL) {
		return 0;
	}
	postorder(root->left);
	postorder(root->right);
	printf("%d ", root->value);
	return 0;
}

int levelOrder(treePos root) {
	if (root == NULL) {
		return 0;
	}
	treePos* queue = (treePos*)malloc(100 * sizeof(treePos));
	if (queue == NULL) {
		printf("Memory allocation failed!\n");
		return -1;
	}
	int capacity = 100;
	int front = 0;
	int rear = 0;
	queue[rear++] = root;
	while (front < rear) {
		if (rear >= capacity) {
			capacity *= 2;
			queue = (treePos*)realloc(queue, capacity * sizeof(treePos));
			if (queue == NULL) {
				printf("Memory allocation failed!\n");
				return -1;
			}
		}
		treePos current = queue[front++];
		printf("%d ", current->value);
		if (current->left != NULL) {
			queue[rear++] = current->left;
		}
		if (current->right != NULL) {
			queue[rear++] = current->right;
		}
	}
	free(queue);
	return 0;
}