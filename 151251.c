void free_bitmaps(struct activity *act[])
{
	int i;

	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->bitmap && act[i]->bitmap->b_array) {
			free(act[i]->bitmap->b_array);
			/* Set pointer to NULL to prevent it from being freed again */
			act[i]->bitmap->b_array = NULL;
		}
	}
}