void free_structures(struct activity *act[])
{
	int i, j;

	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->nr_allocated > 0) {
			for (j = 0; j < 3; j++) {
				if (act[i]->buf[j]) {
					free(act[i]->buf[j]);
					act[i]->buf[j] = NULL;
				}
			}
			act[i]->nr_allocated = 0;
		}
	}
}