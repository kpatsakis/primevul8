void allocate_bitmaps(struct activity *act[])
{
	int i;

	for (i = 0; i < NR_ACT; i++) {
		/*
		 * If current activity has a bitmap which has not already
		 * been allocated, then allocate it.
		 * Note that a same bitmap may be used by several activities.
		 */
		if (act[i]->bitmap && !act[i]->bitmap->b_array) {
			SREALLOC(act[i]->bitmap->b_array, unsigned char,
				 BITMAP_SIZE(act[i]->bitmap->b_size));
		}
	}
}