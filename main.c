#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uk/test.h>
#include <sys/random.h>
#include "test.h"

#define LONG_RUN true

unsigned long randl() {
	unsigned long ret;
	getrandom(&ret, sizeof(ret), 0);
	return ret;
}

int simple_test()
{
	unsigned long middle = randl() % 500;
	unsigned long down = randl() % 12;
	unsigned long up = randl() % 437;

	long idx;

	UK_RADIX_TREE(tree);
	printf("Params: middle-%ld, down-%ld, up-%ld\n", middle, down, up);
	for (idx = -down; idx < up; idx++)
		node_insert(&tree, middle + idx);

	node_check_absent(&tree, middle - down - 1);
	for (idx = -down; idx < up; idx++)
		node_check_present(&tree, middle + idx);
	node_check_absent(&tree, middle + up);

	return 0;
}

UK_TESTCASE(simple_test_testsuite, simple_test_positive)
{
	UK_TEST_EXPECT_SNUM_EQ(simple_test(), 0);
}

// uk_testsuite_register(simple_test_testsuite, NULL);

void __gang_check(unsigned long middle, long down, long up)
{
	long idx;
	UK_RADIX_TREE(tree);

	middle = 1 << 30;
	printf("middle: 0x%lx, down: %ld, up: %ld\n", middle, down, up);

	for (idx = -down; idx < up; idx++)
		node_insert(&tree, middle + idx);

	node_check_absent(&tree, middle - down - 1);
	for (idx = -down; idx < up; idx++)
		node_check_present(&tree, middle + idx);
	node_check_absent(&tree, middle + up);

#ifdef LINUX_RADIX
	node_kill_tree(&tree);
	#endif
}

int gang_check(void)
{
	__gang_check(1UL << 30, 128, 128);
	__gang_check(1UL << 31, 128, 128);
	__gang_check(0xfffffffeUL, 1, 1);

	return 0;
}

UK_TESTCASE(gang_check_testsuite, gang_check_positive)
{
	UK_TEST_EXPECT_SNUM_EQ(gang_check(), 0);
}

uk_testsuite_register(gang_check_testsuite, NULL);

void __big_gang_check(void)
{
	unsigned long start;
	int wrapped = 0;

	start = 0;
	do {
		unsigned long old_start;

//		printf("0x%08lx\n", start);
		__gang_check(start, randl() % 113 + 1, randl() % 71);
		old_start = start;
		start += randl() % 1000000;
		start %= 1ULL << 33;
		if (start < old_start)
			wrapped = 1;
	} while (!wrapped);
}

int big_gang_check(bool long_run)
{
	int i;

	for (i = 0; i < (long_run ? 1000 : 3); i++) {
		__big_gang_check();
		printf("%d ", i);
		fflush(stdout);
	}

	return 0;
}

UK_TESTCASE(big_gang_check_testsuite, big_gang_check_positive)
{
	UK_TEST_EXPECT_SNUM_EQ(big_gang_check(LONG_RUN), 0);
}

// uk_testsuite_register(big_gang_check_testsuite, NULL);

void add_and_check(void)
{
	UK_RADIX_TREE(tree);

	node_insert(&tree, 44);
	node_check_present(&tree, 44);
	node_check_absent(&tree, 43);
	#ifdef LINUX_RADIX
	node_kill_tree(&tree);
#endif
	return 0;
}

#ifdef LINUX_RADIX
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
#endif