#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uk/test.h>
#include "test.h"

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

void __gang_check(unsigned long middle, long down, long up, int chunk, int hop)
{
	long idx;
	UK_RADIX_TREE(tree);

	middle = 1 << 30;

	for (idx = -down; idx < up; idx++)
		node_insert(&tree, middle + idx);

	node_check_absent(&tree, middle - down - 1);
	for (idx = -down; idx < up; idx++)
		node_check_present(&tree, middle + idx);
	node_check_absent(&tree, middle + up);

	if (chunk > 0) {
		node_gang_check_present(&tree, middle - down, up + down,
				chunk, hop);
		node_full_scan(&tree, middle - down, down + up, chunk);
	}
	node_kill_tree(&tree);
}

int gang_check(void)
{
	__gang_check(1UL << 30, 128, 128, 35, 2);
	__gang_check(1UL << 31, 128, 128, 32, 32);
	__gang_check(1UL << 31, 128, 128, 32, 100);
	__gang_check(1UL << 31, 128, 128, 17, 7);
	__gang_check(0xffff0000UL, 0, 65536, 17, 7);
	__gang_check(0xfffffffeUL, 1, 1, 17, 7);

	return 0;
}

// UK_TESTCASE(gang_check_testsuite, gang_check_positive)
// {
// 	UK_TEST_EXPECT_SNUM_EQ(gang_check(), 0);
// }

// uk_testsuite_register(gang_check_testsuite, NULL);

void __big_gang_check(void)
{
	unsigned long start;
	int wrapped = 0;

	start = 0;
	do {
		unsigned long old_start;

//		printf("0x%08lx\n", start);
		__gang_check(start, rand() % 113 + 1, rand() % 71,
				rand() % 157, rand() % 91 + 1);
		old_start = start;
		start += rand() % 1000000;
		start %= 1ULL << 33;
		if (start < old_start)
			wrapped = 1;
	} while (!wrapped);

	return 0;
}

void big_gang_check(bool long_run)
{
	int i;

	for (i = 0; i < (long_run ? 1000 : 3); i++) {
		__big_gang_check();
		printv(2, "%d ", i);
		fflush(stdout);
	}

	return 0;
}

void add_and_check(void)
{
	UK_RADIX_TREE(tree);

	node_insert(&tree, 44);
	node_check_present(&tree, 44);
	node_check_absent(&tree, 43);
	node_kill_tree(&tree);

	return 0;
}

void dynamic_height_check(void)
{
	int i;
	UK_RADIX_TREE(tree);
	tree_verify_min_height(&tree, 0);

	node_insert(&tree, 42);
	tree_verify_min_height(&tree, 42);

	node_insert(&tree, 1000000);
	tree_verify_min_height(&tree, 1000000);

	UK_ASSERT(node_delete(&tree, 1000000));
	tree_verify_min_height(&tree, 42);

	UK_ASSERT(node_delete(&tree, 42));
	tree_verify_min_height(&tree, 0);

	for (i = 0; i < 1000; i++) {
		node_insert(&tree, i);
		tree_verify_min_height(&tree, i);
	}

	i--;
	for (;;) {
		UK_ASSERT(node_delete(&tree, i));
		if (i == 0) {
			tree_verify_min_height(&tree, 0);
			break;
		}
		i--;
		tree_verify_min_height(&tree, i);
	}

	node_kill_tree(&tree);

	return 0;
}