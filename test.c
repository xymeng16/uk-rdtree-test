#include "test.h"
#include <string.h>

struct node *node_create(char *name, unsigned long index){
    struct node *node = malloc(sizeof(struct node));

    strcpy(node->name, name);
    node->index = index;

    return node;
}

int node_insert(struct uk_radix_tree_root *root, unsigned long index);
void node_sanity(struct node *node, unsigned long index);
void node_free(struct node *node, unsigned long index);
int node_delete(struct uk_radix_tree_root *root, unsigned long index);
struct node *node_lookup(struct uk_radix_tree_root *root, unsigned long index);

void node_check_present(struct uk_radix_tree_root *root, unsigned long index);
void node_check_absent(struct uk_radix_tree_root *root, unsigned long index);
void node_gang_check_present(struct uk_radix_tree_root *root, unsigned long start, unsigned long count, int chunk, int hop);
void node_full_scan(struct uk_radix_tree_root *root, unsigned long start, unsigned long count, int chunk);
void node_kill_tree(struct uk_radix_tree_root *root);