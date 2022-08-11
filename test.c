#include <string.h>
#include <stdio.h>

#include "test.h"

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
        printf("uk_radix_tree_insert failed\n");
        free(node);
    }
    return ret;
}

void node_sanity(struct node *node, unsigned long index) {
    unsigned long mask;
    UK_ASSERT(!uk_radix_tree_is_internal_node(node));
    UK_ASSERT(node->order < UK_BITS_PER_LONG);
    mask = (1UL << node->order) - 1;
    UK_ASSERT((node->index | mask) == (index | mask));
}

void node_free(struct node *node, unsigned long index){
    node_sanity(node, index);
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
    node_sanity(node, index);
}

void node_check_absent(struct uk_radix_tree_root *root, unsigned long index) {
    struct node *node = node_lookup(root, index);
    UK_ASSERT(node == NULL);
}

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

        nfound = uk_radix_tree_gang_lookup(root, (void **)nodes, start + into, count_to_find);
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

	while ((nfound = radix_tree_gang_lookup(root, (void **)nodes, into,
					chunk))) {
		for (i = 0; i < nfound; i++) {
			assert(nodes[i]->index == this_index);
			this_index++;
		}
		into = this_index;
	}
	if (chunk)
		assert(this_index == start + count);
	nfound = uk_radix_tree_gang_lookup(root, (void **)nodes,
					this_index, chunk);
	assert(nfound == 0);
}

void node_kill_tree(struct uk_radix_tree_root *root)
{
	struct uk_radix_tree_iter iter;
	void **slot;
	struct node *nodes[32];
	int nfound;

	uk_radix_tree_for_each_slot(slot, root, &iter, 0) {
		if (uk_radix_tree_exceptional_entry(*slot))
			uk_radix_tree_delete(root, iter.index);
	}

	while ((nfound = uk_radix_tree_gang_lookup(root, (void **)nodes, 0, 32))) {
		int i;

		for (i = 0; i < nfound; i++) {
			void *ret;

			ret = radix_tree_delete(root, nodes[i]->index);
			assert(ret == nodes[i]);
			free(nodes[i]);
		}
	}
	assert(radix_tree_gang_lookup(root, (void **)nodes, 0, 32) == 0);
	assert(root->rt_node == NULL);
}

void tree_verify_min_height(struct uk_radix_tree_root *root, int maxindex)
{
	unsigned shift;
	struct uk_radix_tree_node *node = root->rt_node;
	if (!uk_radix_tree_is_internal_node(node)) {
		UK_ASSERT(maxindex == 0);
		return;
	}

	node = entry_to_node(node);
	UK_ASSERT(maxindex <= node_maxindex(node));

	shift = node->shift;
	if (shift > 0)
		UK_ASSERT(maxindex > shift_maxindex(shift - UK_RADIX_TREE_MAP_SHIFT));
	else
		UK_ASSERT(maxindex > 0);
}
