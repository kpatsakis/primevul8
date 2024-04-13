static inline int test_root(ext4_group_t a, int b)
{
	while (1) {
		if (a < b)
			return 0;
		if (a == b)
			return 1;
		if ((a % b) != 0)
			return 0;
		a = a / b;
	}
}