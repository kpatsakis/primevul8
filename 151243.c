void reallocate_all_buffers(struct activity *a, __nr_t nr_min)
{
	int j;
	size_t nr_realloc;

	if (nr_min <= 0) {
		nr_min = 1;
	}
	if (!a->nr_allocated) {
		nr_realloc = nr_min;
	}
	else {
		nr_realloc = a->nr_allocated;
		do {
			nr_realloc = nr_realloc * 2;
		}
		while (nr_realloc < nr_min);
	}

	for (j = 0; j < 3; j++) {
		SREALLOC(a->buf[j], void,
			(size_t) a->msize * nr_realloc * (size_t) a->nr2);
		/* Init additional space which has been allocated */
		if (a->nr_allocated) {
			memset(a->buf[j] + a->msize * a->nr_allocated * a->nr2, 0,
			       (size_t) a->msize * (size_t) (nr_realloc - a->nr_allocated) * (size_t) a->nr2);
		}
	}

	a->nr_allocated = nr_realloc;
}