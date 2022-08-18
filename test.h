#include <uk/radix_tree.h>
#include <string.h>
#include <stdio.h>
struct node {
	unsigned long index;
	unsigned int order;
};

struct node *node_create(unsigned long index, unsigned int order) {
    struct node *node = malloc(sizeof(struct node));

    node->index = index;
    node->order = order;

    return node;
}

int node_insert(struct uk_radix_tree_root *root, unsigned long index) {
    struct node *node = node_create(index, 0);
    int ret = uk_radix_tree_insert(root, index, node);
    if(ret){
        printf("uk_radix_tree_insert(%p, 0x%lx, %p) failed, returned %d, tree height %d\n", root, index, node, ret, root->height);
        free(node);
    }
    return ret;
}

#ifdef LINUX_RADIX
void node_sanity(struct node *node, unsigned long index) {
    unsigned long mask;
    UK_ASSERT(!uk_radix_tree_is_internal_node(node));
    UK_ASSERT(node->order < UK_BITS_PER_LONG);
    mask = (1UL << node->order) - 1;
    UK_ASSERT((node->index | mask) == (index | mask));
}
#endif

void node_free(struct node *node, unsigned long index){
	#ifdef LINUX_RADIX
    node_sanity(node, index);
	#endif
    free(node);
}

int node_delete(struct uk_radix_tree_root *root, unsigned long index) {
    struct node *node = uk_radix_tree_delete(root, index);

    if (!node)
        return 0;

    node_free(node, index);
    return 1;
}

struct node *node_lookup(struct uk_radix_tree_root *root, unsigned long index) {
    return uk_radix_tree_lookup(root, index);
}

void node_check_present(struct uk_radix_tree_root *root, unsigned long index) {
    struct node *node = node_lookup(root, index);
    UK_ASSERT(node != NULL);
	#ifdef LINUX_RADIX
    node_sanity(node, index);
	#endif
}

void node_check_absent(struct uk_radix_tree_root *root, unsigned long index) {
    struct node *node = node_lookup(root, index);
    UK_ASSERT(node == NULL);
}

#ifdef LINUX_RADIX
/*
 * Scan only the passed (start, start+nr] for present items
 */
void node_gang_check_present(struct uk_radix_tree_root *root, unsigned long start, unsigned long count, int chunk, int hop) {
    struct node *nodes[chunk];
    unsigned long into;

    for (into = 0; into < count; ) {
        int nfound;
        int count_to_find = chunk;
        int i;

        if (count_to_find > (count - into))
            count_to_find = count - into;
		printf("start: %lu, count: %lu, into: %lu, count_to_find: %d\n", start, count, into, count_to_find);
        nfound = uk_radix_tree_gang_lookup(root, (void **)nodes, start + into, count_to_find);
		printf("nfound: %d\n", nfound);
        UK_ASSERT(nfound == count_to_find);
        for (i = 0; i < nfound; i++)
            UK_ASSERT(nodes[i]->index == start + into + i);
        into += hop;
    }
}

void node_full_scan(struct uk_radix_tree_root *root, unsigned long start, unsigned long count, int chunk) {
    struct node *nodes[chunk];
	unsigned long into = 0;
	unsigned long this_index = start;
	int nfound;
	int i;

	while ((nfound = uk_radix_tree_gang_lookup(root, (void **)nodes, into,
					chunk))) {
		for (i = 0; i < nfound; i++) {
			UK_ASSERT(nodes[i]->index == this_index);
			this_index++;
		}
		into = this_index;
	}
	if (chunk)
		UK_ASSERT(this_index == start + count);
	nfound = uk_radix_tree_gang_lookup(root, (void **)nodes,
					this_index, chunk);
	UK_ASSERT(nfound == 0);
}
#endif