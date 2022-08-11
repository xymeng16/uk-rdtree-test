#include <uk/radix_tree.h>

struct node {
	char name[20];
	unsigned long index;
};

struct node *node_create(char *name, unsigned long index);
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