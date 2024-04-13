void allocate_structures(struct activity *act[])
{
	int i, j;

	for (i = 0; i < NR_ACT; i++) {
		if (act[i]->nr_ini > 0) {
			for (j = 0; j < 3; j++) {
				SREALLOC(act[i]->buf[j], void,
						(size_t) act[i]->msize * (size_t) act[i]->nr_ini * (size_t) act[i]->nr2);
			}
			act[i]->nr_allocated = act[i]->nr_ini;
		}
	}
}