#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uk/radix_tree.h>
#include <uk/test.h>

// int factorial(int n) {
// 	int result = 1;
// 	for (int i = 1; i <= n; i++) {
// 		result *= i;
// 	}

// 	return result;
// }

// UK_TESTCASE(factorial_testsuite, factorial_test_positive)
// {
// 	UK_TEST_EXPECT_SNUM_EQ(factorial(2), 2);
// }

// uk_testsuite_register(factorial_testsuite, NULL);

/*
 * Radix-tree                                             RADIX_TREE_MAP: 6
 *                                  (root)
 *                                    |
 *                          o---------o---------o
 *                          |                   |
 *                        (0x0)               (0x2)
 *                          |                   |
 *                  o-------o------o            o---------o
 *                  |              |                      |
 *                (0x0)          (0x2)                  (0x2)
 *                  |              |                      |
 *         o--------o------o       |             o--------o--------o
 *         |               |       |             |        |        |
 *       (0x0)           (0x1)   (0x0)         (0x0)    (0x1)    (0x3)
 *         A               B       C             D        E        F
 *
 * A: 0x00000000
 * B: 0x00000001
 * C: 0x00000080
 * D: 0x00080080
 * E: 0x00080081
 * F: 0x00080083
 *
 */

#define NODE_NUM (20)
#define ID_MASK (0x8000)

/* node */
struct node {
	char name[20];
	unsigned long id;
};

/* radix-tree root */
struct uk_radix_tree_root root;

struct node test_node[NODE_NUM];

char *itoa(int num)
{
	char *str = malloc(sizeof(char) * 10);
	sprintf(str, "%d", num);
	return str;
}

void init()
{
	for (int i = 0; i < NODE_NUM; i++) {
		strcpy(test_node[i].name, "ID");
		char *n = itoa(i);
		strcpy(test_node[i].name + 2, n);
		test_node[i].id = ID_MASK | i;
	}
}

int main()
{
	struct node *np;
	struct uk_radix_tree_iter iter;
	void **slot;

	init();
	
	/* Initialize Radix-tree root */
	UK_INIT_RADIX_TREE(&root);

	/* Insert node into Radix-tree */
	for (int i = 0; i < NODE_NUM; i++) {
		uk_radix_tree_insert(&root, test_node[i].id, &test_node[i]);
		printf("Insert node: %s, id: %lu\n", test_node[i].name, test_node[i].id);
	}
	/* Iterate over Radix-tree */
	uk_radix_tree_for_each_slot(slot, &root, &iter, 0) {
		printf("Index: %#lx\n", iter.index);
	}
	

	/* search struct node by id */
	np = uk_radix_tree_lookup(&root, test_node[0].id);
	UK_BUG_ON(!np);
	printf("Radix: %s ID: %lx\n", np->name, np->id);

	/* Delete a node from radix-tree */
	// radix_tree_delete(&root, node0.id);
	// radix_tree_delete(&root, node1.id);
	// radix_tree_delete(&root, node2.id);

	/* Iterate over Radix-tree */
	uk_radix_tree_for_each_slot(slot, &root, &iter, 0) {
		printf("Index: %#lx\n", iter.index);
	}

	return 0;
}