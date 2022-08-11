#include <uk/radix_tree.h>

struct node {
	unsigned long index;
	unsigned int order;
};

struct node *node_create(unsigned long index, unsigned int order);
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
void tree_verify_min_height(struct uk_radix_tree_root *root, int maxindex);